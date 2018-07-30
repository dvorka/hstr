#!/bin/bash
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
