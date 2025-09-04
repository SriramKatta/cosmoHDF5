#!/bin/bash
# Usage: ./check_h5diff.sh file1.hdf5 file2.hdf5

if [ $# -ne 2 ]; then
    echo "Usage: $0 <file1.hdf5> <file2.hdf5>"
    exit 1
fi

FILE1=$(realpath "$1")
FILE2=$(realpath "$2")

# List of datasets to compare
DATASETS=(
    "/PartType1/Coordinates"
    "/PartType1/ParticleIDs"
    "/PartType1/Potential"
    "/PartType1/SubfindDMDensity"
    "/PartType1/SubfindDensity"
    "/PartType1/SubfindHsml"
    "/PartType1/SubfindVelDisp"
    "/PartType1/Velocities"
)

echo "Comparing datasets between $FILE1 and $FILE2"
echo "--------------------------------------------------"

for ds in "${DATASETS[@]}"; do
    echo "Checking $ds ..."
    
    # Capture output & status
    diff_output=$(h5diff "$FILE1" "$FILE2" "$ds" 2>&1)
    status=$?

    if [ $status -eq 0 ]; then
        echo "✅ PASS: $ds"
    elif [ $status -eq 1 ]; then
        echo "❌ FAIL: $ds (differences found)"
        echo "$diff_output"
    else
        echo "⚠️ ERROR: $ds (issue running h5diff)"
        echo "$diff_output"
    fi

    echo "--------------------------------------------------"
done
