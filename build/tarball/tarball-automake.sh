#!/bin/bash
#
# Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>
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

# Script used to prepare HSTR to be build using autotools:
#   --purge ... remove Git and IDE related (meta)files that are commited in Git repository
#               (only transient/temporary files are deleted otherwise)

export AUTOMAKE_LIB=`automake --print-libdir`

function tarballClean() {
    if [ "${1}" = "--purge" ]
    then
	# development files
	rm -vrf build test tests .git .gitignore .settings Debug Release .cproject .project hstr.pro hstr.pro.user snap
	rm -vf .travis.yml pad.xml _config.yml
    fi
    # garbage
    find . -name "*.*~" | xargs rm -vf {}
    rm -rvf atom5te.cache src/.deps
    rm -vf Makefile Makefile.in aclocal.m4 config.log config.status configure depcomp install-sh missing
    rm -vf man/Makefile man/Makefile.in
    rm -vf src/Makefile src/Makefile.in src/hstr src/hh *.o src/*.o 
}

function tarballBuildUsingAutomake() {
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
	echo "ERROR: automake ${AUTOMAKE_LIB} not found!"
	exit 1
    fi

    # detect Windows WSL
    export VENDOR_FILE=/proc/version
    export HINT_FILE=/tmp/hstr-ms-wsl

    if [ -e "${VENDOR_FILE}" ]
    then
	IS_MS=`grep Microsoft ${VENDOR_FILE}`
	if [[ ! -z "${IS_MS}" ]]
	then
	    echo "DETECTED bash on Linux on Windows (MS WSL)"
	    touch ${HINT_FILE}
	else
	    rm -vf ${HINT_FILE}
	fi
    else
	rm -vf ${HINT_FILE}
    fi
}

# ############################################################################
# # Main #
# ############################################################################

# functions expect to be run from HSTR home directory
cd ../..

tarballClean ${1}
tarballBuildUsingAutomake

echo "DONE: ${0} finished in `pwd`"

# eof
