#!/bin/bash

rm -vf *.*~ ../*.*~ ../src/*.*~
rm -rvf ../atom5te.cache
rm -vf ../man/Makefile ../man/Makefile.in
rm -vf ../src/Makefile ../src/Makefile.in ../src/hh ../src/*.o 
rm -rvf ../src/.deps
rm -vf ../Makefile ../Makefile.in ../aclocal.m4 ../config.log ../config.status ../configure ../depcomp ../install-sh ../missing

# eof