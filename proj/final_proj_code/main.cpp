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

  // Step 1: read input on rank 0
  DURATION_MEASURE(read_1_proc, state.island_comm, state.world_comm,
                   header.read_from_file(in_file);
                   dconfig.read_from_file(in_file);
                   params.read_from_file(in_file);
                   parts.setup(header);
                   parts.read_from_file_1proc(in_file, state););

  // Step 2: distribute data to all ranks
  DURATION_MEASURE(scatter_data, state.island_comm, state.world_comm,
                   parts.distribute_data(state.island_comm););
  auto out_file_dir = create_out_files_dir(in_files_dir, state);
  auto outfile = create_parallel_file_handle(out_file_dir, state);

  // Step 3: write output in parallel
  DURATION_MEASURE(write_parallel, state.island_comm, state.world_comm,
                   params.write_to_file(outfile);
                   header.write_to_file(outfile);
                   dconfig.write_to_file(outfile);
                   parts.write_to_file_parallel(outfile, state););

  state.world_comm.iallreduce(&read_1_proc, 1, mpicpp::op::max());
  state.world_comm.iallreduce(&scatter_data, 1, mpicpp::op::max());
  state.world_comm.iallreduce(&write_parallel, 1, mpicpp::op::max());
  int minimun_rank_perisland{};
  state.world_comm.iallreduce(&state.i_size, &minimun_rank_perisland, 1, mpicpp::op::min());

  if (state.w_rank == 0)
  {
    fmt::print("{:d},{:4.3f},{:4.3f},{:4.3f}\n", minimun_rank_perisland, read_1_proc, scatter_data, write_parallel);
  }
  return 0;
}
catch (...)
{
  return exception_handler();
}
