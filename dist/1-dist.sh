#!/bin/bash

export AUTOMAKE_LIB=`automake --print-libdir`

./clean.sh

cd ..
aclocal
automake --force-missing --add-missing
autoconf

rm -rvf autom*te.cache config.sub config.guess

if [ -d ${AUTOMAKE_LIB} ]
then
    rm -vf compile depcomp install-sh missing
    cp -vf ${AUTOMAKE_LIB}/compile .
    cp -vf ${AUTOMAKE_LIB}/depcomp .
    cp -vf ${AUTOMAKE_LIB}/install-sh .
    cp -vf ${AUTOMAKE_LIB}/missing .
    # platform specific detection in configure.ac
    cp -vf ${AUTOMAKE_LIB}/config.sub .
    cp -vf ${AUTOMAKE_LIB}/config.guess .
else 
    echo "****************************************************************"
    echo "Error: automake ${AUTOMAKE_LIB} not found - exiting!"
    exit 1
fi

cd dist

# eof
