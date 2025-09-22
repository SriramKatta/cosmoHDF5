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
    throw std::runtime_error(fmt::format("Not enough MPI ranks({}) available for the number of files({})\n", w_size, numfiles));
  }

  auto island_colour = get_island_colour(w_rank, w_size, numfiles);
  auto islan_comm = world_comm.split(island_colour, w_rank);
  auto i_rank = islan_comm.rank();
  auto i_size = islan_comm.size();

  // if (i_rank == 0)
  //   fmt::print("ranks per island: {}\n", i_size);

  auto ifname = fmt::format("{}/snap_099.{}.hdf5", infiles_dir.string(), island_colour);

  auto root_file_handle = create_parallel_file_handle(outfiles_dir, islan_comm, island_colour);
  auto PartType1 = root_file_handle.createGroup("PartType1");

  DURATION_MEASURE(read1perisland, islan_comm, world_comm,
                   auto total_Coordinates = read_1proc_perisland<double>(ifname, "Coordinates", islan_comm);
                   auto total_Velocities = read_1proc_perisland<float>(ifname, "Velocities", islan_comm);
                   auto total_ParticleIDs = read_1proc_perisland<std::uint64_t>(ifname, "ParticleIDs", islan_comm);
                   auto total_Potential = read_1proc_perisland<float>(ifname, "Potential", islan_comm);
                   auto total_SubfindDMDensity = read_1proc_perisland<float>(ifname, "SubfindDMDensity", islan_comm);
                   auto total_SubfindDensity = read_1proc_perisland<float>(ifname, "SubfindDensity", islan_comm);
                   auto total_SubfindHsml = read_1proc_perisland<float>(ifname, "SubfindHsml", islan_comm);
                   auto total_SubfindVelDisp = read_1proc_perisland<float>(ifname, "SubfindVelDisp", islan_comm););

  DURATION_MEASURE(distributedatadur, islan_comm, world_comm,
                   auto local_Coordinates = distribute_data<3>(total_Coordinates, islan_comm);
                   auto local_Velocities = distribute_data<3>(total_Velocities, islan_comm);
                   auto local_ParticleIDs = distribute_data<1>(total_ParticleIDs, islan_comm);
                   auto local_Potential = distribute_data<1>(total_Potential, islan_comm);
                   auto local_SubfindDMDensity = distribute_data<1>(total_SubfindDMDensity, islan_comm);
                   auto local_SubfindDensity = distribute_data<1>(total_SubfindDensity, islan_comm);
                   auto local_SubfindHsml = distribute_data<1>(total_SubfindHsml, islan_comm);
                   auto local_SubfindVelDisp = distribute_data<1>(total_SubfindVelDisp, islan_comm););

  DURATION_MEASURE(writedatapardur, islan_comm, world_comm,
                   mpi_filldata<3>(PartType1, "Coordinates", local_Coordinates, islan_comm);
                   mpi_filldata<3>(PartType1, "Velocities", local_Velocities, islan_comm);
                   mpi_filldata<1>(PartType1, "ParticleIDs", local_ParticleIDs, islan_comm);
                   mpi_filldata<1>(PartType1, "Potential", local_Potential, islan_comm);
                   mpi_filldata<1>(PartType1, "SubfindDMDensity", local_SubfindDMDensity, islan_comm);
                   mpi_filldata<1>(PartType1, "SubfindDensity", local_SubfindDensity, islan_comm);
                   mpi_filldata<1>(PartType1, "SubfindHsml", local_SubfindHsml, islan_comm);
                   mpi_filldata<1>(PartType1, "SubfindVelDisp", local_SubfindVelDisp, islan_comm);)
  // if (i_rank == 0)
  // {
  //   fmt::print("{:32s} : {:4.3f} s\n", "Time to read data on 1 rank", read1perisland);

  //   fmt::print("{:32s} : {:4.3f} s\n", "Time to distribute data", distributedatadur);

  //   fmt::print("{:32s} : {:4.3f} s\n", "Time to write data in parallel", writedatapardur);
  // }

  world_comm.iallreduce(&read1perisland, 1, mpicpp::op::max());
  world_comm.iallreduce(&distributedatadur, 1, mpicpp::op::max());
  world_comm.iallreduce(&writedatapardur, 1, mpicpp::op::max());
  if (w_rank == 0)
  {
    fmt::print("{:32s} : {:4.3f} s\n", "Time to read data on 1 rank", read1perisland);

    fmt::print("{:32s} : {:4.3f} s\n", "Time to distribute data", distributedatadur);

    fmt::print("{:32s} : {:4.3f} s\n", "Time to write data in parallel", writedatapardur);
  }

  return EXIT_SUCCESS;
}
catch (...)
{
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

// /PartType1/Coordinates   Dataset {23566292, 3}
// /PartType1/Velocities    Dataset {23566292, 3}
// /PartType1/ParticleIDs   Dataset {23566292}
// /PartType1/Potential     Dataset {23566292}
// /PartType1/SubfindDMDensity Dataset {23566292}
// /PartType1/SubfindDensity Dataset {23566292}
// /PartType1/SubfindHsml   Dataset {23566292}
// /PartType1/SubfindVelDisp Dataset {23566292}