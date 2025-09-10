#!/bin/bash -l
#SBATCH --account=punch_astro
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=128
#SBATCH --output=logs/jobID%j.out
#SBATCH --error=logs/jobID%j.err
#SBATCH --time=3:30:0
#SBATCH --partition=dc-cpu

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

# Loop over possible task counts (up to total tasks allocated)
# Adjust max_tasks = nodes * ntasks-per-node
max_tasks=$(( SLURM_NNODES * SLURM_NTASKS_PER_NODE ))

for ntasks in $(seq $count $count $max_tasks); do
for medainrun in {1..10}; do
    echo "==============================================="
    echo "Running with $ntasks tasks median run $medainrun"
    echo "==============================================="
    srun -n $ntasks ./build/bin/read_files_serial "$DIR1"
    echo ""  # blank line for readability
done
done
