#!/bin/bash

. /home/dvorka/p/hstr/github/hstr/dist/env.sh

function branch_hh() {
  bzr branch lp:hstr
  #cd hstr
  #mv .bzr ..
  #rm -rvf debian dist man src LICENSE *.am *.md *.ac
  #mv ../.bzr .
  #cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .
}

function init_hh() {
  cp -rvf ${HHSRC} .
  mv hstr ${HH}
  tar zcf ${HH}.tgz ${HH}
  rm -rvf ${HH}
  bzr dh-make hh ${HHVERSION} ${HH}.tgz
}

rm -rvf *.*~

mkdir ${HHBUILD}
cd ${HHBUILD}
init_hh
cd dist

#echo "Continue by editing debian/changes and dist/ directory: 1-dist.sh"

# eof
