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

# Script used to create TARBALL and to build HSTR using autotools:
#  - run this script from Git repository.
#  - it copies repository content and result to given release directory

export SCRIPT_HOME=`pwd`

export HSTR_VERSION="2.3.0"

export NOW=`date +%Y-%m-%d--%H-%M-%S`
export GH_RELEASE_DIR=~/p/hstr/release
export GH_SRC_DIR=~/p/hstr/github/hstr
export GH_DISTRO_DIR=${GH_RELEASE_DIR}/release-${NOW}

function makeTarballRelease() {
    cp -vrf ${GH_SRC_DIR} .
    mv `basename ${GH_SRC_DIR}` hstr
    cd hstr && rm -vrf debian doc test hstr && cd build/tarball && ./tarball-automake.sh --purge
    if [ ${?} -ne 0 ]
    then
        echo "ERROR: automake prepare phase failed"
        exit 1;
    fi
    cd ../../..
    tar zcfv hstr-${HSTR_VERSION}-tarball.tgz hstr
    cd hstr && ./configure && make && cd src
    tar zcfv ../../hstr-${HSTR_VERSION}-bin-64b.tgz hstr
}

# ############################################################################
# # Main #
# ############################################################################

echo "HSTR tarball and binary builder"
if [ ! -e "../../.git" ]
then
    echo "ERROR: this script must be run FROM Git repository"
    exit 1
fi
if [ ! -e "${GH_SRC_DIR}" ]
then
    echo "ERROR: HSTR sources directory must exist: ${GH_SRC_DIR}"
    exit 1
fi
if [ ! -e "${GH_RELEASE_DIR}" ]
then
    echo "ERROR: release directory must exist: ${GH_RELEASE_DIR}"
    exit 1
fi

mkdir -v ${GH_DISTRO_DIR}
cd ${GH_DISTRO_DIR} && makeTarballRelease

# eof
