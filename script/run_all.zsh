#!/bin/zsh

folder_path="data/input/"

file_paths=($folder_path*)

# TAG here the capacity of each node is same
for file_path in $file_paths; do
    echo "Start $file_path"
    output="data/solution/${file_path:t:r}.sol"
    ./build/capmds "$file_path" -t > "$output"
    wait
    echo "Done $output"
done