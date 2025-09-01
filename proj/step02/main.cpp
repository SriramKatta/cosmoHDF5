#include <fmt/format.h>
#include <unistd.h>
#include <mpicpp.hpp>
#include <tuple>
#include <argparse/argparse.hpp>
#include <filesystem>

namespace fs = std::filesystem;
auto parser(int &argc, char **&argv) -> std::tuple<fs::path, int>
{
    fs::path filedir;
    int island_size;
    argparse::ArgumentParser program("read_files");
    program.add_argument("-d", "--infiles_dir")
        .help("Directory containing input files")
        .default_value(fs::path("."))
        .store_into(filedir);

    program.add_argument("-s", "--island_size")
        .help("Size of islands to create")
        .default_value(1)
        .scan<'i', int>()
        .store_into(island_size);

    program.parse_args(argc, argv);
    return {filedir, island_size};
}

int main(int argc, char **argv)
try
{
    auto env = mpicpp::environment(&argc, &argv);

    auto [infiles_dir, island_size] = parser(argc, argv);

    auto world_comm = mpicpp::comm::world();

    world_comm.barrier();

    auto w_rank = world_comm.rank();
    auto w_size = world_comm.size();
    auto islan_comm = world_comm.split(w_rank / 5, w_rank);
    auto i_rank = islan_comm.rank();
    auto i_size = islan_comm.size();
    MPI_Info obj;

    mpicpp::comm shared_comm = world_comm.split_type(MPI_COMM_TYPE_SHARED, w_rank);
    char host_name[256];
    gethostname(host_name, sizeof(host_name));
    auto s_rank = shared_comm.rank();
    auto s_size = shared_comm.size();
    fmt::print("W_rank {:^3d}/{:^3d} | I_rank {:^3d}/{:^3d} | s_rank {:^3d}/{:^3d} | Host: {:10s}\n", w_rank, w_size, i_rank, i_size, s_rank, s_size, host_name);
}
catch (const std::exception ex)
{
    fmt::print(stderr, "Exception caught in main: {}\n", ex.what());
    return EXIT_FAILURE;
}
