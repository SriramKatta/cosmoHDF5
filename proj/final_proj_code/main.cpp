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

  header_group header;
  config_group dconfig;
  param_group params;
  part_groups parts;

  header.read_from_file(in_file);

  dconfig.read_from_file(in_file);

  params.read_from_file(in_file);

  parts.setup(header);
  parts.read_from_file_1proc(in_file, state);

  parts.distribute_data(state.island_comm);

  auto out_file_dir = create_out_files_dir(in_files_dir, state);
  auto outfile = create_parallel_file_with_groups(out_file_dir, state);

  // Step 3: write output in parallel
  params.write_to_file(outfile);
  header.write_to_file(outfile);
  dconfig.write_to_file(outfile);
  parts.write_to_file_parallel(outfile, state);

  return 0;
}
catch (...)
{
  return exception_handler();
}
