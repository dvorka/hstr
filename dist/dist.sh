#!/bin/bash

./clean.sh

cd ..
aclocal
automake --force-missing --add-missing
autoconf

rm -vf depcomp install-sh missing
cp -vf /usr/share/automake-1.11/depcomp .
cp -vf /usr/share/automake-1.11/install-sh .
cp -vf /usr/share/automake-1.11/missing .

cd dist

# eof
