#pragma once

#include <string>
#include <fmt/format.h>
#include <mpi.h>
#include <unistd.h>
#include <utility>

int get_island_colour(int w_rank, int w_size, int numfiles)
{
  int base_size = w_size / numfiles; // minimum island size
  int remainder = w_size % numfiles; // extra ranks to distribute

  // First `remainder` islands have size base_size+1
  if (w_rank < (base_size + 1) * remainder)
  {
    return w_rank / (base_size + 1);
  }
  else
  {
    int offset = (base_size + 1) * remainder;
    return remainder + (w_rank - offset) / base_size;
  }
}

void debug_print_info(int &w_rank, int &w_size, int &i_rank, int &i_size, std::string &fname)
{
  char host_name[256];
  gethostname(host_name, sizeof(host_name));
  fmt::print("W_rank {:^3d}/{:^3d} | I_rank {:^3d}/{:^3d} | Host: {:s} | will handle {}\n", w_rank, w_size - 1, i_rank, i_size - 1, host_name, fname);
}

template <size_t COLS, typename VT>
std::vector<VT> distribute_data(std::vector<VT> &g_data, const mpicpp::comm &islan_comm)
{
  int rank = islan_comm.rank();
  int size = islan_comm.size();
  int total_entries = g_data.size() / COLS; // logical rows
  int base_entries = total_entries / size;
  int remainder = total_entries % size;
  // Send counts in terms of number of VT elements
  std::vector<int> sendcounts(size, base_entries * COLS);
  std::vector<int> displacements(size, 0);
  for (int i = 0; i < remainder; ++i)
  {
    sendcounts[i] += COLS; // give one extra row (DIM elements) to first `remainder` ranks
  }
  for (int i = 1; i < size; ++i)
  {
    displacements[i] = displacements[i - 1] + sendcounts[i - 1];
  }
  // needed since g_data is filled only on rank 0
  islan_comm.ibcast(sendcounts, 0);
  islan_comm.ibcast(displacements, 0);
  std::vector<VT> local_data(sendcounts[rank]);
  islan_comm.iscatterv(g_data, sendcounts, displacements, local_data, 0);
  return local_data;
}

struct mpi_state
{
  mpicpp::comm island_comm;
  mpicpp::comm world_comm;
  int i_color{-1};
  int i_rank{-1};
  int i_size{-1};
  int w_rank{-1};
  int w_size{-1};
  mpi_state(int numfiles)
  {
    world_comm = mpicpp::comm::world();
    w_rank = world_comm.rank();
    w_size = world_comm.size();
    i_color = get_island_colour(w_rank, w_size, numfiles);
    island_comm = world_comm.split(i_color, w_rank);
    i_rank = island_comm.rank();
    i_size = island_comm.size();
    state_check(numfiles);
  }
  void print(const std::filesystem::path &fname = "")
  {
    fmt::print("W_rank {:^3d}/{:^3d}|I_rank {:^3d}/{:^3d}| i_colour {:^3d}", w_rank, w_size - 1, i_rank, i_size - 1, i_color);
    if (fname.empty())
    {
      fmt::print("\n");
    }
    else
    {
      char host_name[256];
      gethostname(host_name, sizeof(host_name));
      // Get the parent directory name + filename
      std::string result = fname.parent_path().filename().string() + "/" + fname.filename().string();
      fmt::print("| on host {} | will handle {} \n", host_name, result);
    }
  }
  void print_stats()
  {
    if (i_rank == 0)
    {
      fmt::print("Island {:^3d} has {:^3d} ranks per island \n", i_color, i_size);
    }
  }

private:
  void state_check(int numfiles)
  {
    if (w_size < numfiles)
    {
      throw std::runtime_error(fmt::format("Number of MPI ranks ({}) is less than number of files ({}). Exiting...", w_size, numfiles));
    }
  }
};