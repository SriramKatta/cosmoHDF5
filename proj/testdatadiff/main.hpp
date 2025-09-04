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
#include <cmath>

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
    fmt::print(stderr, "Exception type: {}, message: {}\n",
               typeid(e).name(), e.what());
  }
  catch (...)
  {
    fmt::print(stderr, "Caught unknown exception type!\n");
  }

  return EXIT_FAILURE;
}

std::array<std::filesystem::path, 2> parser(int &argc, char **&argv)
{
  namespace fs = std::filesystem;
  fs::path filedir1, filedir2;
  argparse::ArgumentParser program("read_files");
  program.add_argument("infiles_dir1")
      .help("Directory containing input files")
      .store_into(filedir1);

  program.add_argument("infiles_dir2")
      .help("Directory containing input files")
      .store_into(filedir2);

  program.parse_args(argc, argv);
  return {filedir1, filedir2};
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

template <typename VT>
std::vector<VT> read_1proc_perisland(const std::string &ifname, const std::string &datasetname, const mpicpp::comm &island_comm)
{
  std::vector<VT> data;
  if (island_comm.rank() == 0)
  {
    H5::H5File F_root(ifname, H5F_ACC_RDONLY);
    // auto G_header = F_root.openGroup("Header");
    // auto A_numpart_thisfile = G_header.openAttribute("NumPart_ThisFile");
    // auto A_numpart_total = G_header.openAttribute("NumPart_Total");
    // auto ldata_numparts = readAttribute<int>(A_numpart_thisfile);
    // auto gdata_numparts = readAttribute<int>(A_numpart_total);
    // auto l_numparts = ldata_numparts[PARTIDX];
    // auto g_numparts = gdata_numparts[PARTIDX];
    auto G_parttype = F_root.openGroup("PartType1");
    auto D_coordinate = G_parttype.openDataSet(datasetname);
    data = readdataSet<VT>(D_coordinate);
  }
  return data;
}

#define PRINTDIFF(VAR) print_diff(VAR##1, VAR##2, #VAR)

#define ABS(VAL) ((VAL) > 0) ? (VAL) : -(VAL);

template <typename VT>
void print_diff(std::vector<VT> d1, std::vector<VT> d2, const char *dsname)
{
  if (d1.size() != d2.size())
  {
    fmt::print("error in data size {}\n", dsname);
  }
  VT diff{};
  for (size_t i = 0; i < d1.size(); i++)
  {
    diff += ABS(d1[i] - d2[i]);
  }

  fmt::print("{:15s} error is {}\n", dsname, diff);
}