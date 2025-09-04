#pragma once

#include <exception>
#include <mpicpp.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <filesystem>
#include <argparse/argparse.hpp>
#include <unistd.h>
#include <vector>
#include <numeric>
#include <H5Cpp.h>
#include <type_traits>
#include <mpi.h>

constexpr size_t PARTIDX = 1; // since we consider only parttype 1 for now

#define DEBUG_PRINT fmt::print("reached {}\n", __LINE__)

int get_island_colour(int w_rank, int w_size, int numfiles)
{
  int base_size = w_size / numfiles; // minimum island size
  int remainder = w_size % numfiles; // extra ranks to distribute

  // First `remainder` islands have size base_size+1
  if (w_rank < (base_size + 1) * remainder)
  {
    return w_rank / (base_size + 1);
  }
  else
  {
    int offset = (base_size + 1) * remainder;
    return remainder + (w_rank - offset) / base_size;
  }
}

int exception_handler()
{
  try
  {
    throw;
  }
  catch (const std::filesystem::filesystem_error &e)
  {
    fmt::print(stderr, "Filesystem error caught: {}\n", e.what());
  }
  catch (const mpicpp::exception &e)
  {
    fmt::print(stderr, "MPI exception caught: {}\n", e.what());
  }
  catch (const std::runtime_error &e)
  {
    fmt::print(stderr, "Runtime error: {}\n", e.what());
  }
  catch (const H5::Exception &e)
  {
    e.printErrorStack();
  }
  catch (const std::exception &e)
  {
    fmt::print(stderr, "Exception caught: {}\n", e.what());
  }
  catch (...)
  {
    fmt::print(stderr, "Unknown exception caught\n");
  }

  return EXIT_FAILURE;
}

std::filesystem::path parser(int &argc, char **&argv)
{
  namespace fs = std::filesystem;
  fs::path filedir;
  argparse::ArgumentParser program("read_files");
  program.add_argument("infiles_dir")
      .help("Directory containing input files")
      .store_into(filedir);

  program.parse_args(argc, argv);
  return filedir;
}

void debug_print_info(int &w_rank, int &w_size, int &i_rank, int &i_size, std::string &fname)
{
  char host_name[256];
  gethostname(host_name, sizeof(host_name));
  fmt::print("W_rank {:^3d}/{:^3d} | I_rank {:^3d}/{:^3d} | Host: {:s} | will handle {}\n", w_rank, w_size - 1, i_rank, i_size - 1, host_name, fname);
}

int count_hdf5_files(std::filesystem::__cxx11::path &infiles_dir)
{
  namespace fs = std::filesystem;
  int numfiles{0};
  for (auto &i : fs::directory_iterator(infiles_dir))
  {
    auto fname = i.path();
    if (fname.stem().string().find("snap_") != std::string::npos && fname.extension() == ".hdf5")
    {
      ++numfiles;
    }
  }
  if (numfiles == 0)
  {
    auto str = fmt::format("No snap HDF5 files found in directory: {}\n", infiles_dir.string());
    throw std::runtime_error(str);
  }
  return numfiles;
}

std::vector<hsize_t> getextents(const H5::DataSpace &S)
{
  std::vector<hsize_t> extents(S.getSimpleExtentNdims());
  S.getSimpleExtentDims(extents.data());
  return extents;
}

hsize_t gettotalelems(const H5::DataSpace &S)
{
  auto dims = getextents(S);
  return std::accumulate(dims.begin(), dims.end(), 1, std::multiplies<hsize_t>());
}

template <typename T>
std::vector<T> readAttribute(const H5::Attribute &attr)
{
  auto dtype = attr.getDataType();
  auto dspace = attr.getSpace();
  auto totalsize = gettotalelems(dspace);
  std::vector<T> values(totalsize);
  attr.read(dtype, values.data());
  return values;
}

template <typename T>
std::vector<T> readdataSet(const H5::DataSet &D)
{
  auto dtype = D.getDataType();
  auto dspace = D.getSpace();
  auto totalelem = gettotalelems(dspace);
  std::vector<T> data(totalelem);
  D.read(data.data(), dtype, dspace);
  return data;
}

inline H5::FileAccPropList create_mpi_fapl(const mpicpp::comm &comm = mpicpp::comm::world(), MPI_Info info = MPI_INFO_NULL)
{
  hid_t plist_id = H5Pcreate(H5P_FILE_ACCESS);
  H5Pset_fapl_mpio(plist_id, comm.get(), info);
  H5::FileAccPropList fapl(plist_id);
  H5Pclose(plist_id); // safe to close after wrapping
  return fapl;
}

inline H5::DSetMemXferPropList create_mpi_xfer(H5FD_mpio_xfer_t mode = H5FD_MPIO_COLLECTIVE)
{
  hid_t id = H5Pcreate(H5P_DATASET_XFER);
  H5Pset_dxpl_mpio(id, mode);
  H5::DSetMemXferPropList plist(id);
  H5Pclose(id);
  return plist;
}

template <typename VT>
std::vector<VT> read_1proc_perisland(const std::string &ifname, const std::string &datasetname, const mpicpp::comm &island_comm)
{
  std::vector<VT> data;
  if (island_comm.rank() == 0)
  {
    H5::H5File F_root(ifname, H5F_ACC_RDONLY);
    auto G_header = F_root.openGroup("Header");
    auto A_numpart_thisfile = G_header.openAttribute("NumPart_ThisFile");
    auto A_numpart_total = G_header.openAttribute("NumPart_Total");
    auto G_parttype = F_root.openGroup("PartType1");
    auto ldata_numparts = readAttribute<int>(A_numpart_thisfile);
    auto gdata_numparts = readAttribute<int>(A_numpart_total);
    auto l_numparts = ldata_numparts[PARTIDX];
    auto g_numparts = gdata_numparts[PARTIDX];
    auto D_coordinate = G_parttype.openDataSet(datasetname);
    data = readdataSet<VT>(D_coordinate);
  }
  return data;
}

template <size_t COLS, typename VT = double>
std::vector<VT> distribute_data(std::vector<VT> &g_data, const mpicpp::comm &islan_comm)
{

  int rank = islan_comm.rank();
  int size = islan_comm.size();

  int total_entries = g_data.size() / COLS; // logical rows
  int base_entries = total_entries / size;
  int remainder = total_entries % size;

  // Send counts in terms of number of VT elements
  std::vector<int> sendcounts(size, base_entries * COLS);
  std::vector<int> displacements(size, 0);

  for (int i = 0; i < remainder; ++i)
  {
    sendcounts[i] += COLS; // give one extra row (DIM elements) to first `remainder` ranks
  }

  for (int i = 1; i < size; ++i)
  {
    displacements[i] = displacements[i - 1] + sendcounts[i - 1];
  }

  // needed since g_data is filled only on rank 0
  islan_comm.ibcast(sendcounts, 0);
  islan_comm.ibcast(displacements, 0);

  std::vector<VT> local_data(sendcounts[rank]);

  islan_comm.iscatterv(g_data, sendcounts, displacements, local_data, 0);

  return local_data;
}

H5::H5File create_parallel_file_with_groups(const std::filesystem::path &outfiles_dir, const mpicpp::comm &island_comm, const int island_colour)
{
  auto ofname = fmt::format("{}/snap_099.{}.hdf5", outfiles_dir.string(), island_colour);
  auto facc = create_mpi_fapl(island_comm);
  return {ofname, H5F_ACC_TRUNC, facc};
}

template <size_t COLS, typename VT = double>
void mpi_filldata(H5::Group &group, const std::string &datasetname, const std::vector<VT> &data_chunk, mpicpp::comm &island_comm)
{
  const int local_data_size{static_cast<int>(data_chunk.size()) / COLS};
  int global_data_size{0};
  island_comm.iallreduce<int>(&local_data_size, &global_data_size, 1, mpicpp::op::sum());

  // Create a dataset in the specified group
  H5::DataSpace memspace;
  H5::DataSpace filespace;
  if constexpr (COLS == 3)
  {
    std::array<hsize_t, 2> space_mem = {global_data_size / COLS, COLS};
    std::array<hsize_t, 2> space_file = {local_data_size / COLS, COLS};
    filespace = H5::DataSpace(space_file.size(), space_file.data());
  }
  else if constexpr (COLS == 1)
  {
    std::array<hsize_t, 1> space_mem = {global_data_size};
    std::array<hsize_t, 2> space_file = {local_data_size};
    memspace = H5::DataSpace(space_mem.size(), space_mem.data());
    filespace = H5::DataSpace(space_file.size(), space_file.data());
  }

  int start_index = 0;
  int numpart_local = data_chunk.size() / COLS;
  MPI_Exscan(&numpart_local, &start_index, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  if (island_comm.rank() == 0)
    start_index = 0;

  H5::DataSet dataset_handle = group.createDataSet(datasetname, H5::PredType::NATIVE_DOUBLE, memspace);
  // Write the data to the dataset

  if constexpr (COLS == 3)
  {
    std::array<hsize_t, 2> count{static_cast<hsize_t>(numpart_local), 3};
    std::array<hsize_t, 2> start{static_cast<hsize_t>(start_index), 0};
    std::array<hsize_t, 2> stride{1, 1};
    std::array<hsize_t, 2> blocks{1, 1};
    filespace.selectHyperslab(H5S_SELECT_SET, count.data(), start.data(), stride.data(), blocks.data());
  }
  else if (COLS == 1)
  {
    std::array<hsize_t, 1> count{static_cast<hsize_t>(numpart_local)};
    std::array<hsize_t, 1> start{static_cast<hsize_t>(start_index)};
    std::array<hsize_t, 1> stride{1};
    std::array<hsize_t, 1> blocks{1};
    filespace.selectHyperslab(H5S_SELECT_SET, count.data(), start.data(), stride.data(), blocks.data());
  }

  auto transfer_prop = create_mpi_xfer();
  dataset_handle.write(data_chunk.data(), H5::PredType::NATIVE_DOUBLE, memspace, memspace, transfer_prop);
}