#include <fmt/format.h>
#include <mpicpp.hpp>
#include "snap_io.hpp"
#include "general_utils.hpp"
#include "mpi_helpers.hpp"
#include "hdf5_utils.hpp"
#include "main.hpp"

int main(int argc, char **argv)
try
{
  H5::Exception::dontPrint();
  auto in_files_dir = parser(argc, argv);
  int numfiles = count_hdf5_files(in_files_dir);
  mpicpp::environment env(&argc, &argv);
  mpi_state state(numfiles);
  std::filesystem::path in_file_name(fmt::format("{}snap_099.{:d}.hdf5", in_files_dir.string(), state.i_color));
  // state.print(in_file_name);

  auto in_file = H5::H5File(in_file_name.string(), H5F_ACC_RDONLY);
  // all possible part types
  PartType0 pt0;
  PartType1 pt1;
  PartType3 pt3;
  PartType4 pt4;
  PartType5 pt5;

  headerfields header;
  header.read_from_file(in_file);
  // header.print();
  std::array<PartTypeBase *, 6> pts{};
  if (header.NumPart_Total[0] > 0)
    pts[0] = &pt0;
  if (header.NumPart_Total[1] > 0)
    pts[1] = &pt1;
  if (header.NumPart_Total[3] > 0)
    pts[3] = &pt3;
  if (header.NumPart_Total[4] > 0)
    pts[4] = &pt4;
  if (header.NumPart_Total[5] > 0)
    pts[5] = &pt5;

  // Step 1: read datasets on rank 0
  // for (auto *pt : pts)
  // {
  //   if (pt)
  //   {
  //     pt->read_from_file_1proc(in_file, state);
  //   }
  // }

  // // Step 2: distribute data across ranks
  // for (auto *pt : pts)
  // {
  //   if (pt)
  //   {
  //     pt->distribute_data(state.island_comm);
  //   }
  // }

  // Step 3: write output in parallel
  auto out_file_dir = create_out_files_dir(in_files_dir, state);
  auto outfile = create_parallel_file_with_groups(out_file_dir, state);

  header.write_to_file(outfile);

  // for (auto *pt : pts)
  // {
  //   if (pt)
  //   {
  //     pt->write_to_file_parallel(outfile, state);
  //   }
  // }

  return 0;
}
catch (...)
{
  return exception_handler();
}
