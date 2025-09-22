#pragma once

#include <argparse/argparse.hpp>
#include <fmt/format.h>
#include <filesystem>

inline std::filesystem::path parser(int argc, char **argv)
{
    argparse::ArgumentParser program("HDF5 MPI IO");
    program.add_argument("infiles_dir")
        .help("Directory containing input HDF5 files")
        .required();
    program.parse_args(argc, argv);

    auto infiles_dir = std::filesystem::path(program.get<std::string>("infiles_dir"));
    if (!std::filesystem::exists(infiles_dir) || !std::filesystem::is_directory(infiles_dir))
    {
        auto str = fmt::format("Input directory: {} does not exist or is not a directory\n", infiles_dir.string());
        throw std::runtime_error(str);
    }
    return infiles_dir;
}