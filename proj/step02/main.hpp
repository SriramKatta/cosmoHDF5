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
std::vector<VT> read_1proc_perisland(const std::string &ifname, const std::string &datasetname)
{
  std::vector<VT> data;
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
  return data;
}

template <typename VT, size_t DIM>
std::vector<VT> distribute_data(const std::vector<VT> &g_data, const mpicpp::comm &islan_comm)
{
  // throw mpicpp::exception("distribute not implemented right");
#if 0
      auto wrl = mpicpp::comm::world();
      fmt::print("World: rank {}/{} | Island: rank {}/{}\n", wrl.rank(), wrl.size(), islan_comm.rank(), islan_comm.size());
#endif
  int rank = islan_comm.rank();
  int size = islan_comm.size();

  int total_entries = g_data.size() / DIM; // logical rows
  int base_entries = total_entries / size;
  int remainder = total_entries % size;

  // Send counts in terms of number of VT elements
  std::vector<int> sendcounts(size, base_entries * DIM);
  std::vector<int> displs(size, 0);

  for (int i = 0; i < remainder; ++i)
  {
    sendcounts[i] += DIM; // give one extra row (DIM elements) to first `remainder` ranks
  }

  for (int i = 1; i < size; ++i)
  {
    displs[i] = displs[i - 1] + sendcounts[i - 1];
  }

  
  MPI_Datatype mpi_type = MPI_DOUBLE;
  
  mpicpp::handle_error(
    MPI_Bcast(sendcounts.data(), sendcounts.size(), MPI_INT, 0, islan_comm.get())
  );
  mpicpp::handle_error(
    MPI_Bcast(displs.data(), displs.size(), MPI_INT, 0, islan_comm.get())
  );
  
  std::vector<VT> local_data(sendcounts[rank]);

  if (rank == 0)
    fmt::print("send cts : {} | disp : {}\n", sendcounts, displs);
  else
    fmt::print("rank {} |  send cnts of rank {}\n", rank, sendcounts);


    
  mpicpp::handle_error(MPI_Scatterv(
      g_data.data(),
      sendcounts.data(),
      displs.data(),
      mpi_type,
      local_data.data(),
      sendcounts[rank],
      mpi_type,
      0,
      islan_comm.get()));

  return local_data;
}
