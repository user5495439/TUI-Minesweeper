#!/bin/bash

make DEBUG=1
echo ""
gdb build/minesweeper
make clean
