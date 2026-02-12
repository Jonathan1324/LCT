#!/bin/sh

BASE_BUILD="tests/lasm/build"
BASE_LOG="logs/lasm"

find "$BASE_BUILD" -type f -name "*.o" | while read -r file; do
    rel_path="${file#$BASE_BUILD/}"
    arch="$(echo "$rel_path" | cut -d'/' -f1)"
    tool="$(echo "$rel_path" | cut -d'/' -f2)"
    subdir="$(dirname "$rel_path" | cut -d'/' -f3-)"

    log_dir="$BASE_LOG/$arch/objdump-$tool"
    if [ -n "$subdir" ]; then
        log_dir="$log_dir/$subdir"
    fi

    mkdir -p "$log_dir"
    objdump --all-headers "$file" > "$log_dir/$(basename "$file")-objdump.txt" 2>&1
done
