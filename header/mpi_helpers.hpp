#pragma once

#include <string>
#include <fmt/format.h>
#include <mpi.h>

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