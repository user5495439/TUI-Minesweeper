#!/bin/bash

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

chmod +x "$SCRIPT_DIR/build.sh"
"$SCRIPT_DIR/build.sh"
echo ""
chmod +x "$SCRIPT_DIR/build/minesweeper"
"$SCRIPT_DIR/build/minesweeper"
