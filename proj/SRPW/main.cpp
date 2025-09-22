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
  auto outfile_hand = create_parallel_file_handle(out_file_dir, state, H5F_ACC_TRUNC);

  header_group header;
  header.read_from_file_1proc(in_file, state);
  header.distribute_data(state.island_comm);
  header.write_to_file_parallel(outfile_hand);


  config_group dconfig;
  dconfig.read_from_file_1proc(in_file, state);
  dconfig.distribute_data(state.island_comm);
  dconfig.write_to_file_parallel(outfile_hand);

  
  param_group params;
  params.read_from_file_parallel(in_file);
  params.distribute_data(state.island_comm);
  params.write_to_file_parallel(outfile_hand);
  
  
  part_groups parts(header);
  parts.read_from_file_1proc(in_file, state);
  parts.distribute_data(state.island_comm);
  parts.write_to_file_parallel(outfile_hand, state);






  return 0;
}
catch (...)
{
  return exception_handler();
}
