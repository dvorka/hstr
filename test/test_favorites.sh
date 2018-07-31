#!/bin/bash

clear
gcc ./src/test_favorites.c ../src/hstr_favorites.c ../src/hstr_utils.c -o _favorites
cp -vf ./resources/.hh_favorites ~/.hh_favorites
cat ~/.hh_favorites
echo -e "\n-------------\n"
./_favorites
cat ~/.hh_favorites

# eof
