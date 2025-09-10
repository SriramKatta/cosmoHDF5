#pragma once

#include <fmt/format.h>
#include <mpicpp.hpp>
#include <H5Cpp.h>

#define DEBUG_PRINT fmt::print("reached {}\n", __LINE__)

#define DURATION_MEASURE(duration, local_comm, world_comm, codeblock) \
  auto duration##start = MPI_Wtime();                                 \
  codeblock;                                                          \
  auto duration = MPI_Wtime() - duration##start;                      \
  local_comm.iallreduce(&duration, 1, mpicpp::op::max());

int exception_handler()
{
  try
  {
    throw;
  }
  catch (const std::filesystem::filesystem_error &e)
  {
    fmt::print(stderr, "Filesystem error caught: {}\n", e.what());
  }
  catch (const mpicpp::exception &e)
  {
    fmt::print(stderr, "MPI exception caught: {}\n", e.what());
  }
  catch (const std::runtime_error &e)
  {
    fmt::print(stderr, "Runtime error: {}\n", e.what());
  }
  catch (const H5::Exception &e)
  {
    e.printErrorStack();
  }
  catch (const std::exception &e)
  {
    fmt::print(stderr, "Exception caught: {}\n", e.what());
  }
  catch (...)
  {
    fmt::print(stderr, "Unknown exception caught\n");
  }

  return EXIT_FAILURE;
}