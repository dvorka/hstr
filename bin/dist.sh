#!/bin/bash

./clean.sh

cd ..
aclocal
automake --add-missing
autoconf
cd bin

# eof