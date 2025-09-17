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
    auto part0grp = file.openGroup("PartType0");
    // auto coords_ds = part0grp.openDataSet("GFM_MetalsTagged");

    dataset_wattr<double> Coordinates;
    Coordinates.read_dataset_1proc(part0grp, "GFM_WindDMVelDisp", state.i_rank);

    Coordinates.distribute_data(state.island_comm);

    // Coordinates.print();

    auto para_fapl = create_mpi_fapl(state.island_comm);

    auto outfilename = fmt::format("output_{}.hdf5", state.i_color);
    auto out_file_hand = H5::H5File(outfilename, H5F_ACC_TRUNC, para_fapl);

    auto partgrp = out_file_hand.createGroup("PartType0");
    Coordinates.write_to_file_parallel(partgrp, "GFM_WindDMVelDisp", state.island_comm);

    return 0;
}
catch (...)
{
    return exception_handler();
}
