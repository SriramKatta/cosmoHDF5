#!/bin/bash -l
#SBATCH --job-name=bench_run
#SBATCH --account=punch_astro
#SBATCH --ntasks-per-node=128
#SBATCH --output=logs_%x/jobID%j.out
#SBATCH --error=logs_%x/jobID%j.err
#SBATCH --time=0:59:0
#SBATCH --partition=dc-cpu-bigmem

module purge
module load Stages/2025
module load GCC
module load OpenMPI
module load HDF5
module load CMake

# Get directory from command line
DIR1=$(realpath "$1")

# Collect files and count
files=("$DIR1"/snap_099*)
count=${#files[@]}

echo "Found $count files in $DIR1"


