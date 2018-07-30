#!/bin/bash
#
# Copyright (C) 2014-2018  Martin Dvorak <martin.dvorak@mindforger.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
export VENDOR_FILE=/proc/version
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
