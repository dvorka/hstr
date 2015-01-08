#!/bin/bash

# update github-env.sh to put there version and release comment
. ./github-env.sh
export SCRIPT_HOME=`pwd`

function make_github_release() {
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
cd ${GH_DISTRO_DIR} && make_github_release

# eof
