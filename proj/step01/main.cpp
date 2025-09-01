#include <mpi.h>
#include <cstdlib>
#include <fmt/core.h>
#include <unistd.h>

#include <mpicpp.hpp>

int main(int argc, char **argv)
try{
    int num_islands = std::atoi(argv[1]);
    auto env = mpicpp::environment(&argc, &argv);
    auto world_comm = mpicpp::comm::world();
    auto w_rank = world_comm.rank();
    auto w_size = world_comm.size();
    auto islan_comm = world_comm.split(w_rank / num_islands, w_rank);
    auto i_rank = islan_comm.rank();
    auto i_size = islan_comm.size();
    MPI_Info obj;
    

    mpicpp::comm shared_comm= world_comm.split_type(MPI_COMM_TYPE_SHARED, w_rank);
    char host_name[256];
    gethostname(host_name, sizeof(host_name));
    auto s_rank = shared_comm.rank();
    auto s_size = shared_comm.size();
    fmt::print("W_rank {:3d}/{:3d} | I_rank {:3d}/{:3d} | s_rank {:3d}/{:3d} | Host: {:10s}\n", w_rank, w_size, i_rank, i_size, s_rank, s_size, host_name);
}catch(const std::exception ex){
    fmt::print(stderr, "Exception caught in main: {}\n", ex.what());
    return EXIT_FAILURE;

}
