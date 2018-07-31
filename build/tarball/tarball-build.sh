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

# Script used to create TARBALL and to build HSTR using autotools.
# 
# HSTR release method:
#
# 1. Tarball release checklist:
#   - git grep <previous version>
#   - update version in the source code (hstr.c)
#   - update version in configure.ac
#   - update version github-env.sh
#   - ... search for old version using Eclipse/grep
#
# 2. Run this script from Git repository - it will copy repository content
#    to a release directory

export SCRIPT_HOME=`pwd`

export HH_VERSION="1.27.0"

export NOW=`date +%Y-%m-%d--%H-%M-%S`
export GH_RELEASE_DIR=~/p/hstr/release
export GH_DISTRO_DIR=${GH_RELEASE_DIR}/release-${NOW}

function makeTarballRelease() {
    cp -vrf ${SCRIPT_HOME}/../../../hstr .
    cd hstr && rm -vrf debian doc test && cd build/tarball && ./tarball-automake.sh --purge
    if [ ${?} -ne 0 ]
    then
        echo "ERROR: automake prepare phase failed"
        exit 1;
    fi
    cd ../../..
    tar zcfv hh-${HH_VERSION}-tarball.tgz hstr
    cd hstr && ./configure && make && cp src/hh ..
    cd ..
    strip -v hh
    tar zcfv hh-${HH_VERSION}-bin-64b.tgz hh
}

# ############################################################################
# # Main #
# ############################################################################

mkdir -v ${GH_DISTRO_DIR}
cd ${GH_DISTRO_DIR} && makeTarballRelease

# eof
