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

EXE_NAME=$(realpath  "./build/bin/bm_sread_swrite")
TNG_DIR=$(realpath "/p/scratch/punch_astro/katta3/data")
if [ ! -d "$TNG_DIR" ]; then
    echo "Error: $TNG_DIR is not a directory."
    exit 1
fi

# sbatch --nodes=7 --time=3:30:0 ./run_bench_ss.sh $EXE_NAME "${TNG_DIR}/TNG100n455/" 10
# sbatch --nodes=4 --time=3:30:0 ./run_bench_ss.sh $EXE_NAME "${TNG_DIR}/TNG100n455Dark/" 10
# sbatch --nodes=8 --time=3:30:0 ./run_bench_ss.sh $EXE_NAME "${TNG_DIR}/TNG100n910Dark/" 10
# sbatch --nodes=56 --time=8:30:0 ./run_bench_ss.sh $EXE_NAME "${TNG_DIR}/TNG100n910/" 10
# sbatch --nodes=64 --time=8:30:0 ./run_bench_ss.sh $EXE_NAME "${TNG_DIR}/TNG100n1820Dark/" 10
# sbatch --nodes=35 --time=8:30:0 ./run_bench_ss.sh $EXE_NAME "${TNG_DIR}/TNG100n1820/" 10


# watch squeue --me

# --- small helpers ---
submit_block () {
    local nodes=$1
    local path=$2
    local runs=$3
    echo "Submitting BLOCK for $path on $nodes nodes"
    sbatch --nodes=$nodes --time=1:30:0 ./run_bench_ss.sh "$EXE_NAME" "$path" "$runs"
}

submit_cyclic () {
    local nodes=$1
    local path=$2
    local runs=$3
    echo "Submitting CYCLIC for $path on $nodes nodes"
    sbatch --nodes=$nodes --time=1:30:0 ./run_bench_ss.sh "$EXE_NAME" "$path" "$runs" cyclic
}

# --- datasets needing both block + cyclic ---
submit_block 1  "${TNG_DIR}/TNG100n455/"       10
# submit_cyclic 7 "${TNG_DIR}/TNG100n455/"       10

submit_block 1  "${TNG_DIR}/TNG100n455Dark/"   10
# submit_cyclic 4 "${TNG_DIR}/TNG100n455Dark/"   10

submit_block 1  "${TNG_DIR}/TNG100n910Dark/"   10
# submit_cyclic 8 "${TNG_DIR}/TNG100n910Dark/"   10

# --- datasets needing only block ---
submit_block 1 "${TNG_DIR}/TNG100n910/"       10
submit_block 1 "${TNG_DIR}/TNG100n1820Dark/" 10
submit_block 4 "${TNG_DIR}/TNG100n1820/"     10