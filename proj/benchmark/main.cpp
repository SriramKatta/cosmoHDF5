#include "main.hpp"
#include <fmt/format.h>
#include <mpicpp.hpp>
#include "general_utils.hpp"
#include "hdf5_utils.hpp"
#include "mpi_helpers.hpp"
#include "snap_io.hpp"

int main(int argc, char **argv) try {
  H5::Exception::dontPrint();
  auto in_files_dir = parser(argc, argv);
  int numfiles      = count_hdf5_files(in_files_dir);
  mpicpp::environment env(&argc, &argv);
  mpi_state state(numfiles);

  std::filesystem::path p(argv[0]);
  const auto out_dirname = fmt::format("out_{}", p.filename().string());
  auto out_file_dir      = create_out_files_dir(in_files_dir, state, out_dirname);

  header_group header;
  param_group params;
  config_group dconfig;
  part_groups parts;

// --------------------
// Read
// --------------------
#ifdef READ_PARALLEL
  auto in_file = create_parallel_file_handle(in_files_dir, state, H5F_ACC_RDONLY);
#else
  auto in_file = create_serial_file_handle(in_files_dir, state, H5F_ACC_RDONLY);
#endif

  // --------------------
  // Output file handle
  // --------------------
#ifdef WRITE_PARALLEL
  auto outfile_hand = create_parallel_file_handle(out_file_dir, state, H5F_ACC_TRUNC);
#else
  auto outfile_hand = create_serial_file_handle(out_file_dir, state, H5F_ACC_TRUNC);
#endif


#ifdef READ_PARALLEL
  header.read_from_file_parallel(in_file);
  params.read_from_file_parallel(in_file);
  dconfig.read_from_file_parallel(in_file);
  parts.read_from_file_parallel(in_file, state, header);
#else
  header.read_from_file_1proc(in_file, state);
  params.read_from_file_1proc(in_file, state);
  dconfig.read_from_file_1proc(in_file, state);
  parts.read_from_file_1proc(in_file, state, header);
  header.distribute_data(state.island_comm);
  params.distribute_data(state.island_comm);
  dconfig.distribute_data(state.island_comm);
  parts.distribute_data(state.island_comm);
#endif

#ifdef WRITE_PARALLEL
  header.write_to_file_parallel(outfile_hand);
  dconfig.write_to_file_parallel(outfile_hand);
  params.write_to_file_parallel(outfile_hand);
  parts.write_to_file_parallel(outfile_hand, state);
#else

  header.gather_data(state.island_comm);
  dconfig.gather_data(state.island_comm);
  params.gather_data(state.island_comm);
  parts.gather_data(state.island_comm);

  header.write_to_file_1proc(outfile_hand, state);
  dconfig.write_to_file_1proc(outfile_hand, state);
  params.write_to_file_1proc(outfile_hand, state);
  parts.write_to_file_1proc(outfile_hand, state);
#endif

  return 0;
} catch (...) {

  return exception_handler();
}
