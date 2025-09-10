#pragma once

#include <vector>
#include <mpicpp.hpp>
#include <H5Cpp.h>

#include "../step02/main.hpp"

template <size_t COLS, typename VT>
std::vector<VT> gather_data(std::vector<VT> &l_data, const mpicpp::comm &island_comm)
{
    const size_t local_numpart = l_data.size() / COLS;
    size_t global_numpart = 0;

    MPI_Allreduce(&local_numpart, &global_numpart, 1,
                  mpicpp::predefined_datatype<size_t>().get(),
                  MPI_SUM, island_comm.get());

    int local_data_size = static_cast<int>(l_data.size()); // ensure fits
    std::vector<int> recvcounts;
    if (island_comm.rank() == 0) {
        recvcounts.resize(island_comm.size());
    }

    MPI_Gather(&local_data_size, 1, MPI_INT,
               recvcounts.data(), 1, MPI_INT,
               0, island_comm.get());

    std::vector<int> displacements;
    if (island_comm.rank() == 0) {
        displacements.resize(island_comm.size(), 0);
        for (int i = 1; i < island_comm.size(); ++i) {
            displacements[i] = displacements[i - 1] + recvcounts[i - 1] ;
        }
    }

    std::vector<VT> data;
    if (island_comm.rank() == 0) {
        data.resize(global_numpart * COLS);
    }

    MPI_Gatherv(l_data.data(), local_data_size, mpicpp::predefined_datatype<VT>().get(),
                island_comm.rank() == 0 ? data.data() : nullptr,
                recvcounts.data(), displacements.data(),
                mpicpp::predefined_datatype<VT>().get(),
                0, island_comm.get());
    return data;
}


template <size_t COLS, typename VT>
void write_1proc_perisland(const H5::Group &G_parttype, const std::string &datasetname, const std::vector<VT> &local_data, const mpicpp::comm &island_comm)
{

  if (island_comm.rank() == 0)
  {
    std::vector<hsize_t> global_dims;
    if (COLS == 1)
    {
      global_dims = {local_data.size()};
    }
    else
    {
      global_dims = {local_data.size() / COLS, COLS};
    }

    H5::DataSpace dataspace(global_dims.size(), global_dims.data());
    auto dataset_handle = G_parttype.createDataSet(datasetname, get_pred_type<VT>(), dataspace);
    dataset_handle.write(local_data.data(), get_pred_type<VT>());
  }
}

H5::H5File create_serial_file_with_groups(const std::filesystem::path &outfiles_dir, const mpicpp::comm &island_comm, const int island_colour)
{
  if (island_comm.rank() == 0)
  {
    auto ofname = fmt::format("{}/snap_099.{}.hdf5", outfiles_dir.string(), island_colour);
    return {ofname, H5F_ACC_TRUNC};
  }
  return {};
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