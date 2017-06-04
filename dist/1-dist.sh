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

# detect Bash@Ubuntu@Windows
export VENDOR_FILE=/proc/vendor
export HINT_FILE=/tmp/hh-ms-wsl

if [ -e "${VENDOR_FILE}" ]
then
    IS_MS=`grep Microsoft ${VENDOR_FILE}`
    if [[ ! -z "${IS_MS}" ]]
    then
	echo "DETECTED Bash on Linux on Windows (MS WSL)"
	touch ${HINT_FILE}
    else
	rm -vf ${HINT_FILE}
    fi
else
    rm -vf ${HINT_FILE}
fi

cd dist

# eof
