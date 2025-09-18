#!/bin/bash
# Usage: ./check_h5diff.sh DIR1 DIR2

if [ $# -ne 2 ]; then
    echo "Usage: $0 <DIR1> <DIR2>"
    exit 1
fi

DIR1=$1
DIR2=$2

files=("$DIR1"/snap_099*)

echo "All comparisons started on $DIR1"
for f1 in "${files[@]}"; do
(
    fname=$(basename "$f1")
    f2="$DIR2/$fname"

    if [ ! -f "$f2" ]; then
        echo "$fname : ⚠️ missing in $DIR2"
        exit 0
    fi

    diff_output=$(h5diff "$f1" "$f2" 2>&1)
    status=$?

    if [ $status -eq 0 ]; then
        echo "$fname : 🟢 SAME"
    elif [ $status -eq 1 ]; then
        if [ -z "$diff_output" ]; then
            echo "$fname : ✅ PASS (minor issue)"
        else
            echo "$fname : ❌ DIFFERENCES"
        fi
    else
        echo "$fname : ⚠️ ERROR"
    fi
) &
done

wait
echo "All comparisons done."