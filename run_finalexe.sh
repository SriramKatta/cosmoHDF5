#!/bin/bash -l
#SBATCH --job-name=finalexe_run
#SBATCH --account=punch_astro
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=128
#SBATCH --output=logs_out_%x/jobID%j.out
#SBATCH --error=logs_err_%x/jobID%j.err
#SBATCH --time=0:59:0
#SBATCH --partition=dc-cpu-bigmem

module purge
module load Stages/2025
module load GCC
module load OpenMPI
module load HDF5
module load CMake

# Get directory from command line
EXE_NAME=$(realpath "$1")
DIR1=$(realpath "$2")

if [ ! -d "$DIR1" ]; then
    echo "Error: $DIR1 is not a directory."
    exit 1
fi

srun -N $SLURM_NNODES -n $((SLURM_NTASKS_PER_NODE * SLURM_NNODES)) "$EXE_NAME" "${DIR1}/"

echo "Running hdf5diff.sh to compare input and output files..."

./hdf5diff.sh "$DIR1" "$DIR1/out"

echo "Comparison complete."