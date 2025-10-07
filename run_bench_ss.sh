#!/bin/bash -l
#SBATCH --account=punch_astro
#SBATCH --output=bm_logs_ss/bench_%j.out
#SBATCH --error=bm_logs_ss/bench_%j.err
#SBATCH --partition=dc-cpu-devel

module purge
module load Stages/2025
module load GCC
module load OpenMPI
module load HDF5
module load CMake

# --- Arguments ---
if [ $# -lt 3 ]; then
    echo "Usage: sbatch $0 <exe_name> <input_dir> <num_runs> [cyclic]"
    exit 1
fi

EXE_NAME=$(realpath "$1")
INPUT_DIR=$(realpath "$2")/
NUM_RUNS=$3
LAYOUT_MODE=${4:-packed}   # default packed, optional "cyclic"

# --- Build CSV names ---
INPUT_BASENAME=$(basename "$INPUT_DIR")
EXE_BASENAME=$(basename "$EXE_NAME")
CSV_FILE_PACKED="results_ss/${EXE_BASENAME}_${INPUT_BASENAME}_packed.csv"
CSV_FILE_SPREAD="results_ss/${EXE_BASENAME}_${INPUT_BASENAME}_spread.csv"

mkdir -p "$(dirname "$CSV_FILE_PACKED")"

# --- Detect files & cores ---
files=("$INPUT_DIR"/snap_099*)
count=${#files[@]}               # number of files
numproc=$(nproc)                 # cores per node
MAX_CORES=$((SLURM_NNODES * numproc))

echo "Executable: $EXE_NAME"
echo "Found $count files in $INPUT_DIR"
echo "Max cores across nodes: $MAX_CORES"
echo "Layout mode: $LAYOUT_MODE"

# --- Write header function ---
write_header () {
cat > "$1" <<EOF
#layout,nranks,run,minranksperisland, \
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
}
write_header "$CSV_FILE_PACKED"
[ "$LAYOUT_MODE" == "cyclic" ] && write_header "$CSV_FILE_SPREAD"

for i in $(seq 1 $NUM_RUNS); do
  # --- Packed case (always run)
  echo "RUN $i Start"
  srun --distribution=block:block -N ${SLURM_NNODES} -n $count \
       $EXE_NAME "$INPUT_DIR" \
  | awk -v layout="packed" -v r=$count -v run=$i -F, '{print layout","r","run","$0}' \
    >> "$CSV_FILE_PACKED"

  # --- Spread case (only if cyclic mode)
  if [ "$LAYOUT_MODE" == "cyclic" ]; then
    srun --distribution=cyclic --nodes=$count --ntasks=$count --ntasks-per-node=1 \
         $EXE_NAME "$INPUT_DIR" \
    | awk -v layout="spread" -v r=$count -v run=$i -F, '{print layout","r","run","$0}' \
      >> "$CSV_FILE_SPREAD"
  fi
  echo "RUN $i Done"
done

echo "✅ Benchmark finished. Results in:"
echo "   Packed: $CSV_FILE_PACKED"
[ "$LAYOUT_MODE" == "cyclic" ] && echo "   Spread: $CSV_FILE_SPREAD"
