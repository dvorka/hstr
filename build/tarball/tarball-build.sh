#!/bin/bash
#
# Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>
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

# Script used to create TARBALL and to build HSTR binary using autotools:
#  - it copies repository content and result to the release directory
#
# Usage:
#   ./tarball-build.sh               ... maintainer build: run FROM Git repository
#                                        ~/p/hstr/github/hstr/build/tarball, assets are
#                                        created in ~/p/hstr/release/release-<timestamp>/
#   ./tarball-build.sh ci <out-dir>  ... CI build (GitHub Actions): sources are the Git
#                                        repository this script belongs to, assets are
#                                        created in <out-dir> (must be outside of sources)
#
# Assets:
#   hstr-<version>-tarball.tgz ... upstream tarball w/ generated configure
#   hstr-<version>-bin-64b.tgz ... stripped hstr binary

export SCRIPT_HOME=`pwd`
export NOW=`date +%Y-%m-%d--%H-%M-%S`

function fail() {
    echo "ERROR: ${1}"
    exit 1
}

if [ "${1}" = "ci" ]
then
    export HSTR_CI_BUILD=ci
    export GH_SRC_DIR=$(cd "$(dirname "${0}")/../.." && pwd)
    [ -n "${2}" ] || fail "output directory must be specified: ${0} ci <out-dir>"
    # output directory is checked BEFORE it is created
    export GH_RELEASE_DIR=$(realpath -m "${2}")
    case "${GH_RELEASE_DIR}/" in
        "${GH_SRC_DIR}/"*) fail "output directory must be outside of sources: ${GH_RELEASE_DIR}" ;;
    esac
    mkdir -p "${GH_RELEASE_DIR}" || fail "unable to create output directory: ${GH_RELEASE_DIR}"
    export GH_DISTRO_DIR=${GH_RELEASE_DIR}
else
    export HSTR_CI_BUILD=
    export GH_RELEASE_DIR=~/p/hstr/release
    export GH_SRC_DIR=~/p/hstr/github/hstr
    export GH_DISTRO_DIR=${GH_RELEASE_DIR}/release-${NOW}
fi

# version is parsed from hstr.c which is authoritative
export HSTR_VERSION=$(sed -n 's/.*hstr version \\"\([0-9][0-9.]*\)\\".*/\1/p' ${GH_SRC_DIR}/src/hstr.c)
if [ -z "${HSTR_VERSION}" ]
then
    fail "unable to parse HSTR version from ${GH_SRC_DIR}/src/hstr.c"
fi

function makeTarballRelease() {
    # sources are copied to hstr/ - the directory name of the tarball root - only
    # files tracked by Git (w/ working tree changes) are copied, so that ignored
    # and untracked files (build outputs, IDE and agent files, ...) are not released
    mkdir hstr || fail "unable to create hstr/ in `pwd`"
    git -C "${GH_SRC_DIR}" ls-files -z \
        | (cd "${GH_SRC_DIR}" && tar --null --ignore-failed-read -T - -cf -) \
        | tar -xf - -C hstr \
        || fail "unable to copy sources from ${GH_SRC_DIR}"

    cd hstr && \
    rm -vrf debian doc test hstr .git .vscode .github .*~ web && \
    cd build/tarball && \
    ./tarball-automake.sh --purge || fail "automake prepare phase failed"
    cd ../../..

    tar zcfv hstr-${HSTR_VERSION}-tarball.tgz hstr || fail "unable to create tarball"
    cd hstr && ./configure && make || fail "HSTR build failed"
    cd src && strip hstr || fail "unable to strip HSTR binary"
    tar zcfv ../../hstr-${HSTR_VERSION}-bin-64b.tgz hstr || fail "unable to create binary tarball"
    cd ../..
    # CI: build directory is not an asset (it is kept for maintainer builds)
    if [ -n "${HSTR_CI_BUILD}" ]
    then
        rm -rf hstr
    fi
}

# ############################################################################
# # Main #
# ############################################################################

echo "HSTR ${HSTR_VERSION} tarball and binary builder"
if [ -z "${HSTR_CI_BUILD}" ] && [ ! -e "../../.git" ]
then
    fail "this script must be run FROM Git repository"
fi
if [ ! -e "${GH_SRC_DIR}/.git" ]
then
    fail "HSTR Git repository must exist: ${GH_SRC_DIR}"
fi
if [ ! -e "${GH_RELEASE_DIR}" ]
then
    fail "release directory must exist: ${GH_RELEASE_DIR}"
fi

mkdir -pv ${GH_DISTRO_DIR} || fail "unable to create ${GH_DISTRO_DIR}"
cd ${GH_DISTRO_DIR} && makeTarballRelease
echo "DONE: HSTR ${HSTR_VERSION} assets created in ${GH_DISTRO_DIR}:"
ls -l ${GH_DISTRO_DIR}/hstr-${HSTR_VERSION}-*.tgz

# eof
