#pragma once

#include <fmt/format.h>
#include <argparse/argparse.hpp>
#include <filesystem>

inline std::filesystem::path parser(int argc, char **argv) {
  argparse::ArgumentParser program("HDF5 MPI IO");
  program.add_argument("infiles_dir")
    .help("Directory containing input HDF5 files")
    .required();
  program.parse_args(argc, argv);

  auto infiles_dir =
    std::filesystem::path(program.get<std::string>("infiles_dir"));
  if (!std::filesystem::exists(infiles_dir) ||
      !std::filesystem::is_directory(infiles_dir)) {
    auto str =
      fmt::format("Input directory: {} does not exist or is not a directory\n",
                  infiles_dir.string());
    throw std::runtime_error(str);
  }
  return infiles_dir;
}

#define BENCHMARK_VARS                                                      \
  double MIN_para_read_fopen{0.0}, MAX_para_read_fopen{0.0},                \
    AVG_para_read_fopen{0.0}, MIN_seri_read_fopen{0.0},                     \
    MAX_seri_read_fopen{0.0}, AVG_seri_read_fopen{0.0},                     \
    MIN_para_write_fopen{0.0}, MAX_para_write_fopen{0.0},                   \
    AVG_para_write_fopen{0.0}, MIN_seri_write_fopen{0.0},                   \
    MAX_seri_write_fopen{0.0}, AVG_seri_write_fopen{0.0},                   \
    MIN_para_read_headers{0.0}, MAX_para_read_headers{0.0},                 \
    AVG_para_read_headers{0.0}, MIN_seri_read_headers{0.0},                 \
    MAX_seri_read_headers{0.0}, AVG_seri_read_headers{0.0},                 \
    MIN_distribute_header{0.0}, MAX_distribute_header{0.0},                 \
    AVG_distribute_header{0.0}, MIN_para_read_parts{0.0},                   \
    MAX_para_read_parts{0.0}, AVG_para_read_parts{0.0},                     \
    MIN_seri_read_parts{0.0}, MAX_seri_read_parts{0.0},                     \
    AVG_seri_read_parts{0.0}, MIN_distribute_parts{0.0},                    \
    MAX_distribute_parts{0.0}, AVG_distribute_parts{0.0},                   \
    MIN_para_write_headers{0.0}, MAX_para_write_headers{0.0},               \
    AVG_para_write_headers{0.0}, MIN_seri_write_headers{0.0},               \
    MAX_seri_write_headers{0.0}, AVG_seri_write_headers{0.0},               \
    MIN_gather_header{0.0}, MAX_gather_header{0.0}, AVG_gather_header{0.0}, \
    MIN_para_write_parts{0.0}, MAX_para_write_parts{0.0},                   \
    AVG_para_write_parts{0.0}, MIN_seri_write_parts{0.0},                   \
    MAX_seri_write_parts{0.0}, AVG_seri_write_parts{0.0},                   \
    MIN_gather_parts{0.0}, MAX_gather_parts{0.0}, AVG_gather_parts{0.0};

#define BENCHMARK(VAR, STATE, CODE)                         \
  MIN_##VAR         = 0.0;                                 \
  MAX_##VAR         = 0.0;                                 \
  AVG_##VAR         = 0.0;                                 \
  double start##VAR = MPI_Wtime();                         \
  CODE;                                                    \
  const auto VAR = MPI_Wtime() - start##VAR;               \
  STATE.world_comm.iallreduce(&VAR, &MIN_##VAR, 1, mpicpp::op::min()); \
  STATE.world_comm.iallreduce(&VAR, &MAX_##VAR, 1, mpicpp::op::max()); \
  STATE.world_comm.iallreduce(&VAR, &AVG_##VAR, 1, mpicpp::op::sum()); \
  AVG_##VAR /= static_cast<double>(STATE.world_comm.size());
