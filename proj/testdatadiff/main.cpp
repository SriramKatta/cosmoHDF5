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
  auto [infiles_dir1, infiles_dir2] = parser(argc, argv);

  int numfiles = count_hdf5_files(infiles_dir1);

  auto world_comm = mpicpp::comm::world();
  auto w_rank = world_comm.rank();
  auto w_size = world_comm.size();

  if (w_size < numfiles)
  {
    throw std::runtime_error(fmt::format("Not enough MPI ranks({}) available for the number of files({})\n", w_size, numfiles));
  }
  else if (w_rank > numfiles)
  {
    fmt::print("since this is a test code to find diffrence in data it doesnt use the extra rank({}) > files({})\n", w_rank, numfiles);
    return EXIT_SUCCESS;
  }

  auto ifname1 = fmt::format("{}/snap_099.{}.hdf5", infiles_dir1.string(), w_rank);
  auto ifname2 = fmt::format("{}/snap_099.{}.hdf5", infiles_dir2.string(), w_rank);
  if (w_rank == 0)
    fmt::print("file 1 : {}\nfile 2 : {}\n", ifname1, ifname2);
  auto total_Coordinates1 = read_1proc_perisland<double>(ifname1, "Coordinates", world_comm);
  auto total_Velocities1 = read_1proc_perisland<float>(ifname1, "Velocities", world_comm);
  auto total_ParticleIDs1 = read_1proc_perisland<std::uint64_t>(ifname1, "ParticleIDs", world_comm);
  auto total_Potential1 = read_1proc_perisland<float>(ifname1, "Potential", world_comm);
  auto total_SubfindDMDensity1 = read_1proc_perisland<float>(ifname1, "SubfindDMDensity", world_comm);
  auto total_SubfindDensity1 = read_1proc_perisland<float>(ifname1, "SubfindDensity", world_comm);
  auto total_SubfindHsml1 = read_1proc_perisland<float>(ifname1, "SubfindHsml", world_comm);
  auto total_SubfindVelDisp1 = read_1proc_perisland<float>(ifname1, "SubfindVelDisp", world_comm);

  auto total_Coordinates2 = read_1proc_perisland<double>(ifname2, "Coordinates", world_comm);
  auto total_Velocities2 = read_1proc_perisland<float>(ifname2, "Velocities", world_comm);
  auto total_ParticleIDs2 = read_1proc_perisland<std::uint64_t>(ifname2, "ParticleIDs", world_comm);
  auto total_Potential2 = read_1proc_perisland<float>(ifname2, "Potential", world_comm);
  auto total_SubfindDMDensity2 = read_1proc_perisland<float>(ifname2, "SubfindDMDensity", world_comm);
  auto total_SubfindDensity2 = read_1proc_perisland<float>(ifname2, "SubfindDensity", world_comm);
  auto total_SubfindHsml2 = read_1proc_perisland<float>(ifname2, "SubfindHsml", world_comm);
  auto total_SubfindVelDisp2 = read_1proc_perisland<float>(ifname2, "SubfindVelDisp", world_comm);

  PRINTDIFF(total_Coordinates);
  PRINTDIFF(total_Velocities);
  PRINTDIFF(total_ParticleIDs);
  PRINTDIFF(total_Potential);
  PRINTDIFF(total_SubfindDMDensity);
  PRINTDIFF(total_SubfindDensity);
  PRINTDIFF(total_SubfindHsml);
  PRINTDIFF(total_SubfindVelDisp);

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