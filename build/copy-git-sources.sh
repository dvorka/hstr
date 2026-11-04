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

# Copy HSTR sources for a release (tarball, .deb, Launchpad, ...):
#
#   copy-git-sources.sh <Git repository> <destination directory>
#
# Only files tracked by Git are copied (w/ working tree changes) so that ignored
# and untracked files - build outputs, editor backups, IDE and agent files, local
# experiments, ... - never make it to a release. New files must be added to Git
# (git add) to be released. The destination directory is created if it does not
# exist and it must not be inside of the repository.

function fail() {
    echo "ERROR: ${1}" >&2
    exit 1
}

[ $# -eq 2 ] || fail "usage: ${0} <Git repository> <destination directory>"
SRC=$(cd "${1}" 2>/dev/null && pwd) || fail "repository not found: ${1}"
[ -e "${SRC}/.git" ] || fail "not a Git repository: ${SRC}"
DEST=$(realpath -m "${2}")
case "${DEST}/" in
    "${SRC}/"*) fail "destination must be outside of the repository: ${DEST}" ;;
esac

mkdir -p "${DEST}" || fail "unable to create ${DEST}"
# tracked files deleted in the working tree are skipped (--ignore-failed-read)
git -C "${SRC}" ls-files -z \
    | (cd "${SRC}" && tar --null --ignore-failed-read -T - -cf -) \
    | tar -xf - -C "${DEST}" \
    || fail "unable to copy sources from ${SRC} to ${DEST}"
# pipeline status above is the status of the last tar only - check git too
git -C "${SRC}" ls-files --error-unmatch src/hstr.c > /dev/null 2>&1 \
    || fail "unable to list Git tracked files in ${SRC}"
[ -f "${DEST}/src/hstr.c" ] || fail "sources not copied to ${DEST}"

echo "DONE: $(git -C "${SRC}" ls-files | wc -l) Git tracked files copied from ${SRC} to ${DEST}"

# eof
