#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

make DEBUG=1
echo ""
chmod +x "$SCRIPT_DIR/build/minesweeper"
gdb "$SCRIPT_DIR/build/minesweeper"
make clean
