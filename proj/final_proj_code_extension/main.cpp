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
  
  auto out_file_dir = create_out_files_dir(in_files_dir, state);
  // state.print(in_file_name);

  auto in_file = create_serial_file_handle(in_files_dir, state, H5F_ACC_RDONLY);

  header_group header;
  config_group dconfig;
  param_group params;

  header.read_from_file(in_file);
  dconfig.read_from_file(in_file);
  params.read_from_file(in_file);
  part_groups parts(header);
  parts.read_from_file_1proc(in_file, state);

  header.distribute_data(state.island_comm);
  dconfig.distribute_data(state.island_comm);
  params.distribute_data(state.island_comm);
  parts.distribute_data(state.island_comm);


  auto outfile_hand = create_parallel_file_handle(out_file_dir, state, H5F_ACC_TRUNC);

  header.write_to_file(outfile_hand);
  dconfig.write_to_file(outfile_hand);
  params.write_to_file(outfile_hand);
  parts.write_to_file_parallel(outfile_hand, state);


  return 0;
}
catch (...)
{
  return exception_handler();
}
