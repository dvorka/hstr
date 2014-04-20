#!/bin/bash

rm -vf _regexp
gcc ./src/test_regexp.c -o _regexp
./_regexp

# eof
