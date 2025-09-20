#!/bin/bash -l

# just run this script to dispatch all the perf checks

module purge
module load Stages/2025
module load GCC
module load OpenMPI
module load HDF5
module load CMake

# cmake --fresh -S. -B build
cmake --build build -- -j

TNG_DIR=$(realpath "$1")
if [ ! -d "$TNG_DIR" ]; then
  echo "Error: $TNG_DIR is not a directory."
  exit 1
fi

sbatch --nodes=4 --time=4:30:0 ./run_bench.sh "${TNG_DIR}/TNG100n455Dark/" 10 455Dark.csv
sbatch --nodes=4 --time=4:30:0 ./run_bench.sh "${TNG_DIR}/TNG100n455/" 10 455norm.csv
sbatch --nodes=4 --time=4:30:0 ./run_bench.sh "${TNG_DIR}/TNG100n910Dark/" 10 910Dark.csv
sbatch --nodes=4 --time=4:30:0 ./run_bench.sh "${TNG_DIR}/TNG100n910/" 10 910norm.csv
sbatch --nodes=8 --time=4:30:0 ./run_bench.sh "${TNG_DIR}/TNG100n1820Dark/" 10 1820Dark.csv
sbatch --nodes=8 --time=4:30:0 ./run_bench.sh "${TNG_DIR}/TNG100n1820/" 10 1820norm.csv


watch squeue --me