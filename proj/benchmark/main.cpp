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
  auto out_file_dir = create_out_files_dir(in_files_dir, state, out_dirname);

  header_group header;
  param_group params;
  config_group dconfig;
  part_groups parts;

  BENCHMARK_VARS;

// --------------------
// Read
// --------------------
#ifdef READ_PARALLEL
  BENCHMARK(para_read_fopen, state,
            auto in_file = create_parallel_file_handle(in_files_dir, state,
                                                       H5F_ACC_RDONLY););
#else
  BENCHMARK(seri_read_fopen, state,
            auto in_file =
              create_serial_file_handle(in_files_dir, state, H5F_ACC_RDONLY););
#endif

  // --------------------
  // Output file handle
  // --------------------
#ifdef WRITE_PARALLEL
  BENCHMARK(para_write_fopen, state,
            auto outfile_hand =
              create_parallel_file_handle(out_file_dir, state, H5F_ACC_TRUNC););
#else
  BENCHMARK(seri_write_fopen, state,
            auto outfile_hand =
              create_serial_file_handle(out_file_dir, state, H5F_ACC_TRUNC););
#endif


#ifdef READ_PARALLEL
  BENCHMARK(para_read_headers, state, {
    header.read_from_file_parallel(in_file);
    params.read_from_file_parallel(in_file);
    dconfig.read_from_file_parallel(in_file);
  });

  BENCHMARK(para_read_parts, state,

            { parts.read_from_file_parallel(in_file, state, header); });
#else
  BENCHMARK(seri_read_headers, state, {
    header.read_from_file_1proc(in_file, state);
    params.read_from_file_1proc(in_file, state);
    dconfig.read_from_file_1proc(in_file, state);
  });

  BENCHMARK(distribute_header, state, {
    header.distribute_data(state.island_comm);
    params.distribute_data(state.island_comm);
    dconfig.distribute_data(state.island_comm);
  });

  BENCHMARK(seri_read_parts, state,
            parts.read_from_file_1proc(in_file, state, header););

  BENCHMARK(distribute_parts, state,
            { parts.distribute_data(state.island_comm); });
#endif


#ifdef WRITE_PARALLEL
  BENCHMARK(para_write_headers, state, {
    header.write_to_file_parallel(outfile_hand);
    dconfig.write_to_file_parallel(outfile_hand);
    params.write_to_file_parallel(outfile_hand);
  });

  BENCHMARK(para_write_parts, state,
            { parts.write_to_file_parallel(outfile_hand, state); });
#else

  BENCHMARK(gather_header, state, {
    header.gather_data(state.island_comm);
    dconfig.gather_data(state.island_comm);
    params.gather_data(state.island_comm);
  });

  BENCHMARK(gather_parts, state, { parts.gather_data(state.island_comm); });


  BENCHMARK(seri_write_headers, state, {
    header.write_to_file_1proc(outfile_hand, state);
    dconfig.write_to_file_1proc(outfile_hand, state);
    params.write_to_file_1proc(outfile_hand, state);
  });

  BENCHMARK(seri_write_parts, state,
            { parts.write_to_file_1proc(outfile_hand, state); });
#endif

  auto size_island = state.island_comm.size();
  int min_island_size{0};
  state.world_comm.iallreduce(&size_island, &min_island_size, 1,
                              mpicpp::op::min());

  if (state.w_rank == 0) {
    fmt::print(
      "{:3d},"
      "{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},"
      "{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},"
      "{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},"
      "{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},"
      "{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},"
      "{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f},{:5.3f}\n",
      min_island_size, MIN_para_read_fopen, MAX_para_read_fopen,
      AVG_para_read_fopen, MIN_seri_read_fopen, MAX_seri_read_fopen,
      AVG_seri_read_fopen, MIN_para_write_fopen, MAX_para_write_fopen,
      AVG_para_write_fopen, MIN_seri_write_fopen, MAX_seri_write_fopen,
      AVG_seri_write_fopen, MIN_para_read_headers, MAX_para_read_headers,
      AVG_para_read_headers, MIN_seri_read_headers, MAX_seri_read_headers,
      AVG_seri_read_headers, MIN_distribute_header, MAX_distribute_header,
      AVG_distribute_header, MIN_para_read_parts, MAX_para_read_parts,
      AVG_para_read_parts, MIN_seri_read_parts, MAX_seri_read_parts,
      AVG_seri_read_parts, MIN_distribute_parts, MAX_distribute_parts,
      AVG_distribute_parts, MIN_para_write_headers, MAX_para_write_headers,
      AVG_para_write_headers, MIN_seri_write_headers, MAX_seri_write_headers,
      AVG_seri_write_headers, MIN_gather_header, MAX_gather_header,
      AVG_gather_header, MIN_para_write_parts, MAX_para_write_parts,
      AVG_para_write_parts, MIN_seri_write_parts, MAX_seri_write_parts,
      AVG_seri_write_parts, MIN_gather_parts, MAX_gather_parts,
      AVG_gather_parts);
  }

  return 0;
} catch (...) {

  return exception_handler();
}
