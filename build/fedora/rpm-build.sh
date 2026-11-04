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

# Build native Fedora binary and source RPMs from the upstream tarball:
#
#   ./rpm-build.sh <tarball> <out-dir>
#
# - <tarball> is hstr-<version>-tarball.tgz created by build/tarball/tarball-build.sh
# - RPMs are built for the Fedora version this script runs on e.g. hstr-3.3.0-1.fc45.x86_64.rpm
#   and hstr-3.3.0-1.fc45.src.rpm (GitHub Actions run it in fedora:<version> containers)
# - prerequisites: dnf install rpm-build gcc make ncurses-devel readline-devel bash-completion-devel
#
# This script does NOT need root - RPMs are built in a temporary rpmbuild tree.

function fail() {
    echo "ERROR: ${1}"
    exit 1
}

export HSTRSRC=$(cd "$(dirname "${0}")/../.." && pwd)

[ -f "${1}" ] && [ -n "${2}" ] || fail "usage: ${0} <upstream tarball> <out-dir>"
command -v rpmbuild > /dev/null || fail "rpmbuild not found - install rpm-build"
ARG_TARBALL=$(cd "$(dirname "${1}")" && pwd)/$(basename "${1}")
mkdir -p "${2}" && ARG_OUTDIR=$(cd "${2}" && pwd) || fail "unable to create ${2}"

# version is parsed from hstr.c which is authoritative
export HSTR_VERSION=$(sed -n 's/.*hstr version \\"\([0-9][0-9.]*\)\\".*/\1/p' ${HSTRSRC}/src/hstr.c)
[ -n "${HSTR_VERSION}" ] || fail "unable to parse HSTR version from ${HSTRSRC}/src/hstr.c"
[ "$(basename "${ARG_TARBALL}")" = "hstr-${HSTR_VERSION}-tarball.tgz" ] \
    || fail "tarball must be hstr-${HSTR_VERSION}-tarball.tgz: ${ARG_TARBALL}"

TOPDIR=$(mktemp -d "${TMPDIR:-/tmp}/hstr-rpm.XXXXXX") || fail "unable to create rpmbuild tree"
trap 'rm -rf "${TOPDIR}"' EXIT
mkdir -p "${TOPDIR}"/{BUILD,RPMS,SOURCES,SPECS,SRPMS} || fail "unable to create rpmbuild tree"

cp "${ARG_TARBALL}" "${TOPDIR}/SOURCES/" || fail "unable to copy ${ARG_TARBALL}"
# RPM changelog date must be in C locale e.g. Wed Nov 04 2026
CHANGELOG_DATE=$(LC_ALL=C date "+%a %b %d %Y")
sed -e "s/@HSTR_VERSION@/${HSTR_VERSION}/g" \
    -e "s/@HSTR_CHANGELOG_DATE@/${CHANGELOG_DATE}/g" \
    "${HSTRSRC}/build/fedora/hstr.spec" > "${TOPDIR}/SPECS/hstr.spec" \
    || fail "unable to create spec"

echo "Building HSTR ${HSTR_VERSION} RPMs in ${TOPDIR}"
rpmbuild --define "_topdir ${TOPDIR}" -ba "${TOPDIR}/SPECS/hstr.spec" || fail "rpmbuild failed"

# binary and source RPM only - debuginfo and debugsource RPMs are not release assets
RPMS=$(find "${TOPDIR}/RPMS" "${TOPDIR}/SRPMS" -name "hstr-${HSTR_VERSION}-*.rpm")
[ -n "${RPMS}" ] || fail "no RPMs built in ${TOPDIR}"
cp -v ${RPMS} "${ARG_OUTDIR}/" || fail "unable to copy RPMs to ${ARG_OUTDIR}"
echo "DONE: HSTR ${HSTR_VERSION} RPMs created in ${ARG_OUTDIR}:"
ls -l "${ARG_OUTDIR}"/hstr-${HSTR_VERSION}-*.rpm

# eof
