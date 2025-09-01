#include <fmt/format.h>
#include <unistd.h>
#include <mpicpp.hpp>
#include <tuple>
#include <argparse/argparse.hpp>
#include <filesystem>
#include "main.h"

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
    catch (std::exception &e)
  {
    fmt::print(stderr, "Exception caught: {}\n", e.what());
  }
  return EXIT_FAILURE;
}

namespace fs = std::filesystem;
fs::path parser(int &argc, char **&argv)
{
  fs::path filedir;
  argparse::ArgumentParser program("read_files");
  program.add_argument("infiles_dir")
      .help("Directory containing input files")
      .default_value(fs::path("."))
      .store_into(filedir);

  program.parse_args(argc, argv);
  return filedir;
}

int main(int argc, char **argv)
try
{
  auto env = mpicpp::environment(&argc, &argv);

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
      fmt::print(stderr, "Not enough MPI ranks({}) available for the number of files({})\n", w_size, numfiles);
    }
    return EXIT_FAILURE;
  }
  auto computed_island_size = w_size / numfiles;
  auto island_key = w_rank / computed_island_size;
  auto islan_comm = world_comm.split(island_key, w_rank);
  auto i_rank = islan_comm.rank();
  auto i_size = islan_comm.size();

  auto fname = fmt::format("{}/snap_099.{}.hdf5", infiles_dir.string(), island_key);
  if (i_rank == 0)
  {
    
  }
  
}
catch (...)
{
  return exception_handler();
}

int count_hdf5_files(std::filesystem::__cxx11::path &infiles_dir)
{
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
    fmt::print(stderr, "No snap HDF5 files found in directory: {}\n", infiles_dir.string());
    throw std::runtime_error("No snap HDF5 files found");
  }
  return numfiles;
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
