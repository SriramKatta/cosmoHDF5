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
  auto out_file_dir = in_files_dir / "out";
  int numfiles = count_hdf5_files(in_files_dir);
  mpicpp::environment env(&argc, &argv);
  mpi_state state(numfiles);

  // state.print(in_file_name);

  auto in_file = create_serial_file_handle(in_files_dir, state, H5F_ACC_RDONLY);

  header_group header;
  // config_group dconfig;
  // param_group params;
  // part_groups parts;

  // Step 1: read input on rank 0
  header.read_from_file_1proc(in_file, state);

  // header.print();
  header.distribute_data(state.island_comm);

  auto outfile_hand = create_serial_file_handle(out_file_dir, state, H5F_ACC_TRUNC);

  header.write_to_file_1proc(outfile_hand, state);

  // DEBUG_PRINT;
  // params.write_to_file(outfile);
  // dconfig.write_to_file(outfile);
  // parts.write_to_file_parallel(outfile, state);

  return 0;
}
catch (...)
{
  return exception_handler();
}
