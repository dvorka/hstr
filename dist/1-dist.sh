#!/bin/bash

./clean.sh

cd ..
aclocal
automake --force-missing --add-missing
autoconf

rm -rvf autom*te.cache

if [ -d /usr/share/automake-1.11 ]
then
    cp -vf /usr/share/automake-1.11/depcomp .
    cp -vf /usr/share/automake-1.11/install-sh .
    cp -vf /usr/share/automake-1.11/missing .
fi

cd dist

# eof
