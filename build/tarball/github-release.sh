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
#
# Method:
# 1. Tarball release checklist:
#   - git grep <previous version>
#   - update version in the source code (hstr.c)
#   - update version in configure.ac
#   - update version github-env.sh
#   - ... search for old version using Eclipse/grep
#
# 2. Run this script from this directory.
#

. ./tarball-env.sh
export SCRIPT_HOME=`pwd`

function make_tarball_release() {
    cp -vrf ${SCRIPT_HOME}/../../hstr .
    cd hstr && rm -vrf debian doc tests && cd dist && ./1-dist.sh
    if [ $? -ne 0 ]
    then
       exit 1;
    fi
    cd ../..
    tar zcfv hh-${HHVERSION}-src.tgz hstr
    cd hstr && ./configure && make && cp src/hh ..
    cd ..
    tar zcfv hh-${HHVERSION}-bin-64b.tgz hh
}


mkdir -v ${GH_DISTRO_DIR}
cd ${GH_DISTRO_DIR} && make_tarball_release

# eof
