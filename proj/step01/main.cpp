#include <mpi.h>
#include <cstdlib>
#include <fmt/core.h>
#include <unistd.h>

#include <mpicpp.hpp>

int main(int argc, char **argv)
{
    auto env = mpicpp::environment(&argc, &argv);

    auto comm = mpicpp::comm::world();

    int world_size = comm.rank();
    int world_rank = comm.size();

    
    if (argc < 2)
    {
        if (world_rank == 0)
        {
            fmt::print(stderr, "Usage: mpirun -n <num_ranks> ./islands <num_islands>\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int num_islands = std::atoi(argv[1]);

    if (num_islands <= 0)
    {
        if (world_rank == 0)
        {
            fmt::print(stderr, "Number of islands must be > 0.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Fail if more islands requested than ranks
    if (num_islands > world_size)
    {
        if (world_rank == 0)
        {
            fmt::print(stderr,
                       "Error: Requested {} islands but only {} ranks available.\n",
                       num_islands, world_size);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    // Compute balanced block distribution
    int base_size = world_size / num_islands;
    int remainder = world_size % num_islands;

    int start = 0, island_id = -1;
    for (int i = 0; i < num_islands; ++i)
    {
        int size = base_size + (i < remainder ? 1 : 0);
        int end = start + size;
        if (world_rank >= start && world_rank < end)
        {
            island_id = i;
            break;
        }
        start = end;
    }

    // Create subcommunicator
    MPI_Comm island_comm;
    MPI_Comm_split(MPI_COMM_WORLD, island_id, world_rank, &island_comm);

    MPI_Comm sharemem_comm;
    MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, world_rank, MPI_INFO_NULL, &sharemem_comm);

    int island_rank, island_size;
    MPI_Comm_rank(island_comm, &island_rank);
    MPI_Comm_size(island_comm, &island_size);

    int smem_rank, smem_size;
    MPI_Comm_rank(sharemem_comm, &smem_rank);
    MPI_Comm_size(sharemem_comm, &smem_size);

    char hname[256];
    gethostname(hname, sizeof(hname));
    

    fmt::print("World Rank {:2d} -> Island {:2d} (Island Rank {:2d}/{:2d}) | sharedmem region {:2d}/{:2d} | on host {}\n",
               world_rank, island_id, island_rank, island_size - 1, smem_rank, smem_size - 1, hname);

    int flag = (smem_rank == 0) ? 1 : 0;
    int smemregions{0};
    MPI_Allreduce(&flag, &smemregions, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    if (world_rank == 0)
    {
        fmt::print("total numer of shared mem regions {}", smemregions);
    }
    
    MPI_Comm_free(&island_comm);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
