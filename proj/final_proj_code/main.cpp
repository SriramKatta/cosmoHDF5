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
  int numfiles  = count_hdf5_files(in_files_dir);
  mpicpp::environment env(&argc, &argv);
  mpi_state state(numfiles);
  std::filesystem::path in_file_name(fmt::format("{}snap_099.{:d}.hdf5", in_files_dir.string(), state.i_color));
  // state.print(in_file_name);
  
  // read data
  auto in_file = create_parallel_file_with_groups(in_files_dir, state, H5F_ACC_RDONLY);
  auto header = read_header(in_file);
  





  // distribute data

  // write data
  auto out_file_dir = create_out_files_dir(in_files_dir, state);
  auto outfile = create_parallel_file_with_groups(out_file_dir, state);
  auto header_group = outfile.createGroup("Header");
  write_header(header_group, header);

  return 0;
}
catch (...)
{
  return exception_handler();
}
