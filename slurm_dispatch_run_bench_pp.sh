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

EXE_NAME=$(realpath  "./build/bin/bm_pread_pwrite")
TNG_DIR=$(realpath "/p/scratch/punch_astro/katta3/data")
if [ ! -d "$TNG_DIR" ]; then
    echo "Error: $TNG_DIR is not a directory."
    exit 1
fi

sbatch --nodes=1 --time=4:30:0 ./run_bench_pp.sh $EXE_NAME "${TNG_DIR}/TNG100n455/" 10
sbatch --nodes=1 --time=4:30:0 ./run_bench_pp.sh $EXE_NAME "${TNG_DIR}/TNG100n455Dark/" 10
sbatch --nodes=1 --time=4:30:0 ./run_bench_pp.sh $EXE_NAME "${TNG_DIR}/TNG100n910Dark/" 10
sbatch --nodes=5 --time=4:30:0 ./run_bench_pp.sh $EXE_NAME "${TNG_DIR}/TNG100n910/" 10
sbatch --nodes=5 --time=4:30:0 ./run_bench_pp.sh $EXE_NAME "${TNG_DIR}/TNG100n1820Dark/" 10
sbatch --nodes=35 --time=8:30:0 ./run_bench_pp.sh $EXE_NAME "${TNG_DIR}/TNG100n1820/" 10


# watch squeue --me