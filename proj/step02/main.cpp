#include <fmt/format.h>
#include <mpicpp.hpp>
#include <tuple>
#include <filesystem>
#include <H5Cpp.h>
#include "main.hpp"

namespace fs = std::filesystem;

int main(int argc, char **argv)
try
{
  auto env = mpicpp::environment(&argc, &argv);
  H5::Exception::dontPrint();
  auto infiles_dir = parser(argc, argv);

  int numfiles = count_hdf5_files(infiles_dir);

  auto world_comm = mpicpp::comm::world();
  auto w_rank = world_comm.rank();
  auto w_size = world_comm.size();


  auto outfiles_dir = infiles_dir / "out";
  if (w_rank == 0)
  {
    fs::create_directories(outfiles_dir);
  }

  if (w_size < numfiles)
  {
    if (w_rank == 0)
    {
      throw std::runtime_error(fmt::format("Not enough MPI ranks({}) available for the number of files({})\n", w_size, numfiles));
    }
    return EXIT_FAILURE;
  }

  auto island_colour = get_island_colour(w_rank, w_size, numfiles);
  auto islan_comm = world_comm.split(island_colour, w_rank);
  auto i_rank = islan_comm.rank();
  auto i_size = islan_comm.size();

  auto ifname = fmt::format("{}/snap_099.{}.hdf5", infiles_dir.string(), island_colour);
  fmt::print("numfiles {} | {}\n",numfiles, ifname);


  std::vector<double> total_coordinates;

  debug_print_info(w_rank, w_size, i_rank, i_size, ifname);

  if (i_rank == 0)
  {
    total_coordinates = read_1proc_perisland<double>(ifname, "Coordinates");
  }

  world_comm.barrier();
  auto local_data = distribute_data<double, 3>(total_coordinates, islan_comm);
  #if 1
  

    auto ofname = fmt::format("{}/snap_099.{}.hdf5", outfiles_dir.string(), island_colour);
    auto facc = create_mpi_fapl(islan_comm);
    H5::H5File testFile(ofname, H5F_ACC_TRUNC, facc);
    std::array<hsize_t, 2> dims{static_cast<hsize_t>(total_coordinates.size()), 3};
    H5::DataSpace filespace(2, dims.data(), NULL);
    auto dataset_handle = testFile.createDataSet("Coordinates", H5::PredType::NATIVE_DOUBLE, filespace);
    int start_index = 0;
    int numpart_local = local_data.size();
    MPI_Exscan(&numpart_local, &start_index, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (i_rank == 0)
      start_index = 0;

    std::array<hsize_t, 2> count{static_cast<hsize_t>(numpart_local), 3};
    std::array<hsize_t, 2> start{static_cast<hsize_t>(start_index), 0};
    std::array<hsize_t, 2> stride{1, 1};
    std::array<hsize_t, 2> blocks{1, 1};
    filespace.selectHyperslab(H5S_SELECT_SET, count.data(), start.data(), stride.data(), blocks.data());
    std::vector<hsize_t> tds{total_coordinates.size(), 3};
    auto memspace = H5::DataSpace(tds.size(), tds.data(), NULL);

    auto prop = create_mpi_xfer();

    dataset_handle.write(local_data.data(), H5::PredType::NATIVE_DOUBLE, memspace, filespace, prop);
  #endif

  return EXIT_SUCCESS;
}
catch (...)
{
  fmt::print(stderr, "exception caught\n");
  return exception_handler();
}

// proper way bu too much work for now
// fs::path fullfilename;
// for (auto &i : fs::directory_iterator(infiles_dir))
// {
//   if (i.path().extension() == ".hdf5")
//   {
//     fullfilename = i.path(); // e.g. "snap_099.7.hdf5"
//     break;
//   }
// }

// auto filename = fullfilename.stem().stem(); // since format is in <fname>.<rank>.hdf5
// auto outfilename
