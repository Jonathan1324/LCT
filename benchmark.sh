#!/bin/bash
# Usage: ./benchmark.sh <relative_path_to_asm_file>

set -e

ASM_FILE="$1"
if [ -z "$ASM_FILE" ]; then
    echo "Usage: $0 <asm_file_relative_to_tests/lasm/srcs>"
    exit 1
fi

SRC_PATH="tests/lasm/srcs/$ASM_FILE"
OUT_DIR="build/tests"
OUT_FILE_LASM="$OUT_DIR/speed_lasm.out"
OUT_FILE_NASM="$OUT_DIR/speed_nasm.out"

mkdir -p "$OUT_DIR"

if command -v hyperfine >/dev/null 2>&1; then
    hyperfine --warmup 5 --prepare "rm -f $OUT_FILE_LASM; rm -rf $OUT_FILE_NASM" \
        "dist/bin/lasm $SRC_PATH -o $OUT_FILE_NASM --format bin --arch x86" \
        "nasm $SRC_PATH -o $OUT_FILE_LASM -fbin"
else
    echo "Hyperfine not found, using 'time' as fallback:"
    echo "LASM:"
    time dist/bin/lasm "$SRC_PATH" -o "$OUT_FILE" --format bin --arch x86
    echo "NASM:"
    time nasm "$SRC_PATH" -o "$OUT_FILE" -fbin
fi
