#!/bin/bash -l
#SBATCH --account=punch_astro
#SBATCH --output=bm_logs_pp/bench_%j.out
#SBATCH --error=bm_logs_pp/bench_%j.err
#SBATCH --partition=dc-cpu-bigmem   

module purge
module load Stages/2025
module load GCC
module load OpenMPI
module load HDF5
module load CMake

# --- Arguments ---
if [ $# -ne 3 ]; then
    echo "Usage: sbatch $0 <exe_name> <input_dir> <num_runs>"
    exit 1
fi

EXE_NAME=$(realpath "$1")
INPUT_DIR=$(realpath "$2")/
NUM_RUNS=$3

# --- Build CSV name ---
INPUT_BASENAME=$(basename "$INPUT_DIR")
EXE_BASENAME=$(basename "$EXE_NAME")
CSV_FILE="results_pp/${EXE_BASENAME}_${INPUT_BASENAME}.csv"

mkdir -p "$(dirname "$CSV_FILE")"

# --- Detect files & cores ---
files=("$INPUT_DIR"/snap_099*)
count=${#files[@]}               # number of files
numproc=$(nproc)                 # cores per node
MAX_CORES=$((SLURM_NNODES * numproc))

echo "Executable: $EXE_NAME"
echo "Found $count files in $INPUT_DIR"
echo "Max cores across nodes: $MAX_CORES"
echo "CSV output: $CSV_FILE"

# --- Write header ---
cat > "$CSV_FILE" <<EOF
#nranks,run,minranksperisland, \
MIN_para_read_fopen,MAX_para_read_fopen,AVG_para_read_fopen, \
MIN_seri_read_fopen,MAX_seri_read_fopen,AVG_seri_read_fopen, \
MIN_para_write_fopen,MAX_para_write_fopen,AVG_para_write_fopen, \
MIN_seri_write_fopen,MAX_seri_write_fopen,AVG_seri_write_fopen, \
MIN_para_read_headers,MAX_para_read_headers,AVG_para_read_headers, \
MIN_seri_read_headers,MAX_seri_read_headers,AVG_seri_read_headers, \
MIN_distribute_header,MAX_distribute_header,AVG_distribute_header, \
MIN_para_read_parts,MAX_para_read_parts,AVG_para_read_parts, \
MIN_seri_read_parts,MAX_seri_read_parts,AVG_seri_read_parts, \
MIN_distribute_parts,MAX_distribute_parts,AVG_distribute_parts, \
MIN_para_write_headers,MAX_para_write_headers,AVG_para_write_headers, \
MIN_seri_write_headers,MAX_seri_write_headers,AVG_seri_write_headers, \
MIN_gather_header,MAX_gather_header,AVG_gather_header, \
MIN_para_write_parts,MAX_para_write_parts,AVG_para_write_parts, \
MIN_seri_write_parts,MAX_seri_write_parts,AVG_seri_write_parts, \
MIN_gather_parts,MAX_gather_parts,AVG_gather_parts
EOF

# --- Loop over multiples of count ---
for ((ranks=$count; ranks<=MAX_CORES; ranks+=count)); do
    echo "▶️ Testing with $ranks ranks (multiple of numfiles=$count)"
    
    for i in $(seq 1 $NUM_RUNS); do
        echo -n "   Run $i ..."
        srun  --distribution=block:block  --cpu-bind=map_cpu:$(seq -s, 0 $(($(nproc)-1))) \
        -N ${SLURM_NNODES} -n $ranks $EXE_NAME "$INPUT_DIR" \
        | awk -v r=$ranks -v run=$i -F, '{print r","run","$0}' >> "$CSV_FILE"
    done
done

echo "✅ Benchmark finished. Results in $CSV_FILE"
