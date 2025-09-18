#include <fmt/format.h>
#include <mpicpp.hpp>
#include <tuple>
#include <filesystem>
#include <H5Cpp.h>
#include "../step02/main.hpp"
#include "snap_io.hpp"

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
    throw std::runtime_error(fmt::format("Not enough MPI ranks({}) available for the number of files({})\n", w_size, numfiles));
  }

  auto island_colour = get_island_colour(w_rank, w_size, numfiles);
  auto islan_comm = world_comm.split(island_colour, w_rank);
  auto i_rank = islan_comm.rank();
  auto i_size = islan_comm.size();

  // if (i_rank == 0)
  //   fmt::print("ranks per island: {}\n", i_size);

  auto ifname = fmt::format("{}/snap_099.{}.hdf5", infiles_dir.string(), island_colour);

  auto read_file = H5::H5File(ifname, H5F_ACC_RDONLY);
  auto rgrp = read_file.openGroup("/Header");

  header_group header_data;

  header_data.read_from_group(rgrp);

  auto writefile = create_parallel_file_with_groups(outfiles_dir, islan_comm, island_colour);
  auto GRP = writefile.openGroup("/Header");
  header_data.write_to_group(GRP);

  return EXIT_SUCCESS;
}
catch (...)
{
  return exception_handler();
}
