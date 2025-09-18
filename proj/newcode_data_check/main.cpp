#include <argparse/argparse.hpp>

#include "snap_io.hpp"
#include "general_utils.hpp"

int main(int argc, char **argv)
try
{
    H5::Exception::dontPrint();
    mpicpp::environment env(&argc, &argv);
    std::filesystem::path input_file;
    argparse::ArgumentParser program("newcode_data_check");
    program.add_argument("input_file").help("Input file to read").store_into(input_file);

    program.parse_args(argc, argv);

    // int numfiles = count_hdf5_files(input_file);
    mpi_state state(1);

    H5::H5File file(input_file.string(), H5F_ACC_RDONLY);

    // auto coords_ds = part0grp.openDataSet("GFM_MetalsTagged");
    PartType0 pt0;

    pt0.read_from_file_1proc(file, state);
    pt0.distribute_data(state.island_comm);
    // pt0.print();

    auto para_fapl = create_mpi_fapl(state.island_comm);

    auto outfilename = fmt::format("output_{}.hdf5", state.i_color);
    auto out_file_hand = H5::H5File(outfilename, H5F_ACC_TRUNC, para_fapl);
    pt0.write_to_file_parallel(out_file_hand, state);

    return 0;
}
catch (...)
{
    return exception_handler();
}
