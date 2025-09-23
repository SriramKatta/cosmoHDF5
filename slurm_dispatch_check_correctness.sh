#!/bin/bash -l

# just run this script to dispatch all the correctness checks
# results are place in logs_finalexe_run folder
# revies the .out files to see if everything is ok
# usage: ./slurm_dispatch_check_correctness.sh /path/to/TNG
# example: ./slurm_dispatch_check_correctness.sh /p/scratch/punch_astro/katta3/data

module purge
module load Stages/2025
module load GCC
module load OpenMPI
module load HDF5
module load CMake

# cmake --fresh -S. -B build
cmake --build build -- -j

EXE_NAME=$(realpath "$1")

TNG_DIR=$(realpath "$2")
if [ ! -d "$TNG_DIR" ]; then
    echo "Error: $TNG_DIR is not a directory."
    exit 1
fi

echo "using the exe $EXE_NAME"

sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "$EXE_NAME" "${TNG_DIR}/TNG100n455/"
echo "${TNG_DIR}/TNG100n455/"

sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "$EXE_NAME" "${TNG_DIR}/TNG100n455Dark/"
echo "${TNG_DIR}/TNG100n455Dark/"

sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "$EXE_NAME" "${TNG_DIR}/TNG100n910/"
echo "${TNG_DIR}/TNG100n910/"

sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "$EXE_NAME" "${TNG_DIR}/TNG100n910Dark/"
echo "${TNG_DIR}/TNG100n910Dark/"

sbatch --nodes=4 --time=0:30:0 run_finalexe.sh "$EXE_NAME" "${TNG_DIR}/TNG100n1820/"
echo "${TNG_DIR}/TNG100n1820/"

sbatch --nodes=4 --time=0:30:0 run_finalexe.sh "$EXE_NAME" "${TNG_DIR}/TNG100n1820Dark/"
echo "${TNG_DIR}/TNG100n1820Dark/"

watch squeue --me
