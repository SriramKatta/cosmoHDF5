#!/bin/bash -l
#SBATCH --account=punch_astro
#SBATCH --output=logs/bench_%j.out
#SBATCH --error=logs/bench_%j.err
#SBATCH --time=1:00:00
#SBATCH --partition=dc-cpu-bigmem   # adjust partition if needed

module purge
module load Stages/2025
module load GCC
module load OpenMPI
module load HDF5
module load CMake

# --- Arguments ---
if [ $# -ne 3 ]; then
    echo "Usage: sbatch $0 <input_dir> <num_runs> <output_csv>"
    exit 1
fi

INPUT_DIR=$(realpath "$1")/
NUM_RUNS=$2
CSV_FILE=$3


# --- Detect files & cores ---
files=("$INPUT_DIR"/snap_099*)
count=${#files[@]}              # number of files
numproc=$(nproc)                # number of cores on this node
MAX_CORES=$((SLURM_NNODES * numproc))         # max ranks available from Slurm

echo "Found $count files in $INPUT_DIR"
echo "Max cores on this node: $MAX_CORES"

# --- Prepare output ---
mkdir -p "$(dirname "$CSV_FILE")"
echo "#nranks,run,minranksperisland,read,scatter,write" > "$CSV_FILE"

# --- Loop over multiples of count ---
for ((ranks=$count; ranks<=MAX_CORES; ranks+=count)); do
    echo "▶️ Testing with $ranks ranks (multiple of numfiles($count))"
    
    for i in $(seq 1 $NUM_RUNS); do
        echo -n "   Run $i ..."
        srun -N 1 -n $ranks ./build/bin/final "$INPUT_DIR" \
        | awk -v r=$ranks -v run=$i -F, '{print r","run","$0}' | tee -a "$CSV_FILE"
    done
done

echo "✅ Benchmark finished. Results in $CSV_FILE"
