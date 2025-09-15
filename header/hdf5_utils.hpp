#pragma once

#include <H5Cpp.h>
#include <type_traits>
#include <filesystem>
#include <mpicpp.hpp>
#include <mpi_helpers.hpp>

template <typename VT>
constexpr H5::PredType get_pred_type()
{
  if constexpr (std::is_same_v<VT, double>)
    return H5::PredType::IEEE_F64LE;
  else if constexpr (std::is_same_v<VT, float>)
    return H5::PredType::IEEE_F32LE;
  else if constexpr (std::is_same_v<VT, std::uint64_t>)
    return H5::PredType::STD_U64LE;
  else if constexpr (std::is_same_v<VT, std::uint32_t>)
    return H5::PredType::STD_U32LE;
  else if constexpr (std::is_same_v<VT, std::int64_t>)
    return H5::PredType::STD_I64LE;
  else if constexpr (std::is_same_v<VT, std::int32_t>)
    return H5::PredType::STD_I32LE;
  else
    static_assert(!sizeof(VT), "Unsupported type for get_pred_type");
}

int count_hdf5_files(std::filesystem::path &infiles_dir)
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

H5::H5File create_parallel_file_with_groups(const std::filesystem::path &outfiles_dir, const mpicpp::comm &island_comm, const int island_colour, unsigned int flags = H5F_ACC_TRUNC)
{
  auto ofname = fmt::format("{}/snap_099.{}.hdf5", outfiles_dir.string(), island_colour);
  auto facc = create_mpi_fapl(island_comm);
  return {ofname, flags, facc};
}

H5::H5File create_parallel_file_with_groups(const std::filesystem::path &outfiles_dir, const mpi_state& state, unsigned int flags = H5F_ACC_TRUNC) {
  return create_parallel_file_with_groups(outfiles_dir, state.island_comm, state.i_color, flags);
}

