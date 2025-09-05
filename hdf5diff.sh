#!/bin/bash
# Usage: ./check_h5diff.sh DIR1 DIR2

if [ $# -ne 2 ]; then
    echo "Usage: $0 <DIR1> <DIR2>"
    exit 1
fi

DIR1=$1
DIR2=$2

# Collect snap_099* files from DIR1
files=("$DIR1"/snap_099*)

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

for f1 in "${files[@]}"; do
    fname=$(basename "$f1")
    f2="$DIR2/$fname"

    if [ ! -f "$f2" ]; then
        echo "⚠️ Skipping $fname: not found in $DIR2"
        continue
    fi

    echo "=================================================="
    echo "Comparing datasets between:"
    echo "  $f1"
    echo "  $f2"
    echo "=================================================="

    for ds in "${DATASETS[@]}"; do
        echo "Checking $ds ..."
        diff_output=$(h5diff "$f1" "$f2" "$ds" 2>&1)
        status=$?

        if [ $status -eq 0 ]; then
            echo "🟢 PERFECT LY SAME: $ds (no differences listed)"
        elif [ $status -eq 1 ]; then
            if [ "$diff_output" == "" ]; then
                    echo "✅ DATA OKAY PASS MAYBE SOME OTHER ISSUE: $ds"       
            else
                echo "❌ FAIL: $ds (differences found)"
                echo "$diff_output"
            fi
        else
            echo "⚠️ ERROR: $ds (issue running h5diff)"
            echo "$diff_output"
        fi
        echo "--------------------------------------------------"
    done
done
