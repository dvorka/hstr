#!/bin/bash

valgrind --tool=memcheck --leak-check=full ../src/hstr

# eof
