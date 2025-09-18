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

TNG_DIR=$(realpath "$1")
if [ ! -d "$TNG_DIR" ]; then
  echo "Error: $TNG_DIR is not a directory."
  exit 1
fi


sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "${TNG_DIR}/TNG100n455Dark/"

sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "${TNG_DIR}/TNG100n455/"

sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "${TNG_DIR}/TNG100n910Dark/"

sbatch --nodes=2 --time=0:30:0 run_finalexe.sh "${TNG_DIR}/TNG100n910/"

sbatch --nodes=4 run_finalexe.sh "${TNG_DIR}/TNG100n1820Dark/"

sbatch --nodes=4 run_finalexe.sh "${TNG_DIR}/TNG100n1820/"
    

watch squeue --me
