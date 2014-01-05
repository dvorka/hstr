#!/bin/bash

. /home/dvorka/p/hstr/github/hstr/dist/env.sh

function checkout_hh() {
  bzr checkout lp:~ultradvorka/+junk/hh-package
  #bzr branch lp:~ultradvorka/+junk/hh-package
  #bzr clone lp:~ultradvorka/+junk/hh-package
  #bzr branch lp:hstr
  mv hh-package ${HH}
  cd ${HH}
  mv .bzr ..
  rm -rvf debian dist man src LICENSE *.am *.md *.ac auto*.*
  mv ../.bzr .
  cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .
  cd ..
  #tar zcf ${HH}.tgz ${HH}
  #cp -vf ${H}.tgz ${HH}.orig.tar.gz
  mv -v ${HH} hh
  cd ./hh/dist && ./1-dist.sh && cd ../..
}

function init_hh() {
  cp -rvf ${HHSRC} .
  cd ./hstr/dist && ./1-dist.sh && rm -vrf ../debian && cd ../..
  cd ${1}
  mv hstr ${HH}
  tar zcf ${HH}.tgz ${HH}
  rm -rvf ${HH}
  bzr dh-make -v hh ${HHVERSION} ${HH}.tgz
}

rm -rvf *.*~ ./debian

mkdir ${HHBUILD}
cd ${HHBUILD}

checkout_hh `pwd`
#init_hh `pwd`

cd hh/dist
./2-deb.sh

pwd
echo -e "\n\nContinue by editing debian/changes and dist/ directory: 3-deb-push.sh\n"

# eof
