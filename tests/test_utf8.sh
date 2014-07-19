#!/bin/bash

clear
rm -vf ./_utf8
echo "Linking wide version of Curses library (libncursesw5-dev)..."
gcc ./src/test_utf8.c -o _utf8 -lncursesw
./_utf8

# eof

