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

# Launchpad release checklist:
# - update version in source code (hstr.c)
# - update version in configure.ac
# - update release message in this file
# - update version in this file
# - check target Ubuntu distributions in this file
# - check minor version in this file
# - run this script from ~/p/hstr/launchpad

if [ -e "../.git" ]
then
  echo "This script MUST NOT be run from Git repository - run it e.g. from ~/p/hstr/launchpad instead"
  exit 1
fi

export HHBZRMSG="Top and bottom prompt."
export VERSION=1.25.
export MINOR=1

# precise quantal saucy precise utopic vivid wily yakkety zesty / trusty xenial artful bionic
for DISTRO in trusty xenial artful bionic
do

./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"
MINOR=`expr $MINOR + 1`

done

# eof
