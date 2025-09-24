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

  // --------------------
  // Input file handle
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

  // --------------------
  // HEADER
  // --------------------
  header_group header;
#ifdef READ_PARALLEL
  header.read_from_file_parallel(in_file);
#else
  header.read_from_file_1proc(in_file, state);
  header.distribute_data(state.island_comm);
#endif

#ifdef WRITE_PARALLEL
  header.write_to_file_parallel(outfile_hand);
#else
  header.gather_data(state.island_comm);
  header.write_to_file_1proc(outfile_hand, state);
#endif

  // --------------------
  // CONFIG
  // --------------------
  config_group dconfig;
#ifdef READ_PARALLEL
  dconfig.read_from_file_parallel(in_file);
#else
  dconfig.read_from_file_1proc(in_file, state);
  dconfig.distribute_data(state.island_comm);
#endif

#ifdef WRITE_PARALLEL
  dconfig.write_to_file_parallel(outfile_hand);
#else
  dconfig.gather_data(state.island_comm);
  dconfig.write_to_file_1proc(outfile_hand, state);
#endif

  // --------------------
  // PARAMS
  // --------------------
  param_group params;
#ifdef READ_PARALLEL
  params.read_from_file_parallel(in_file);
#else
  params.read_from_file_1proc(in_file, state);
  params.distribute_data(state.island_comm);
#endif

#ifdef WRITE_PARALLEL
  params.write_to_file_parallel(outfile_hand);
#else
  params.gather_data(state.island_comm);
  params.write_to_file_1proc(outfile_hand, state);
#endif

  // --------------------
  // PARTICLES
  // --------------------
  part_groups parts(header);
#ifdef READ_PARALLEL
  parts.read_from_file_parallel(in_file, state);
#else
  parts.read_from_file_1proc(in_file, state);
  parts.distribute_data(state.island_comm);
#endif

#ifdef WRITE_PARALLEL
  parts.write_to_file_parallel(outfile_hand, state);
#else
  parts.gather_data(state.island_comm);
  parts.write_to_file_1proc(outfile_hand, state);
#endif

  return 0;
} catch (...) {

  return exception_handler();
}
