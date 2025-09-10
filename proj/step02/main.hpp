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
#include "attribute_helper.hpp"
#include "general_utils.hpp"
#include "hdf5_utils.hpp"
#include "mpi_helpers.hpp"

constexpr size_t PARTIDX = 1; // since we consider only parttype 1 for now


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
    auto G_parttype = F_root.openGroup("PartType1");
    auto D_coordinate = G_parttype.openDataSet(datasetname);
    data = readdataSet<VT>(D_coordinate);
  }
  return data;
}


H5::H5File create_parallel_file_with_groups(const std::filesystem::path &outfiles_dir, const mpicpp::comm &island_comm, const int island_colour)
{
  auto ofname = fmt::format("{}/snap_099.{}.hdf5", outfiles_dir.string(), island_colour);
  auto facc = create_mpi_fapl(island_comm);
  return {ofname, H5F_ACC_TRUNC, facc};
}



template <size_t COLS, typename VT>
void mpi_filldata(H5::Group &group,
                  const std::string &datasetname,
                  const std::vector<VT> &data_chunk,
                  mpicpp::comm &island_comm)
{
  // Local number of rows
  const hsize_t local_rows = data_chunk.size() / COLS;

  // Global number of rows
  hsize_t global_rows = 0;
  island_comm.iallreduce(&local_rows, &global_rows, 1, mpicpp::op::sum());

  // Define global dataset shape
  std::vector<hsize_t> global_dims;
  if (COLS == 1)
  {
    global_dims = {global_rows};
  }
  else
  {
    global_dims = {global_rows, COLS};
  }

  H5::DataSpace file_space(global_dims.size(), global_dims.data());

  // Create dataset with *global* dimensions
  auto h5dt = get_pred_type<VT>();

  H5::DataSet dataset_handle =
      group.createDataSet(datasetname, h5dt, file_space);

  // Define memory space (local buffer shape)
  std::vector<hsize_t> mem_dims;
  if (COLS == 1)
  {
    mem_dims = {local_rows};
  }
  else
  {
    mem_dims = {local_rows, COLS};
  }
  H5::DataSpace mem_space(mem_dims.size(), mem_dims.data());

  // Compute starting row offset for each rank
  hsize_t start_row = 0;
  MPI_Exscan(&local_rows, &start_row, 1, MPI_LONG_LONG, MPI_SUM, island_comm.get());
  if (island_comm.rank() == 0)
    start_row = 0;

  // Select hyperslab in global file space
  if constexpr (COLS == 1)
  {
    std::array<hsize_t, 1> start{start_row};
    std::array<hsize_t, 1> count{local_rows};
    file_space.selectHyperslab(H5S_SELECT_SET, count.data(), start.data());
  }
  else
  {
    std::array<hsize_t, 2> start{start_row, 0};
    std::array<hsize_t, 2> count{local_rows, COLS};
    file_space.selectHyperslab(H5S_SELECT_SET, count.data(), start.data());
  }

  // Collective parallel write
  auto transfer_prop = create_mpi_xfer();
  dataset_handle.write(data_chunk.data(),
                       h5dt,
                       mem_space, file_space, transfer_prop);

  // write_dataset_attribute<VT>(dataset_handle);
}


