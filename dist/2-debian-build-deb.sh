#!/bin/bash

export SCRIPTHOME=`pwd`

. ./debian-env.sh

function createChangelog() {
  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo -e "hh (${HHFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo -e "\n" >> $1
  echo -e "  * ${HHBZRMSG}" >> $1
  echo -e "\n" >> $1
  echo -e " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com> ${MYTS} +0100" >> $1
  echo -e "\n" >> $1
}

function createTarball() {
  cd ..
  mkdir work
  cd work
  cp -vrf ../${HH} .
  rm -rvf ${HH}/.bzr
  tar zcf ../${HH}.tgz ${HH}
  cp -vf ../${HH}.tgz ../${HH}.orig.tar.gz
  cd ../${HH}
}

echo -e "\n_ hh deb build  _______________________________________________\n"

rm -rvf ../debian
cp -rvf ${HHSRC}/debian ..

createChangelog ../debian/changelog
cp -vf debian/rules ../debian/rules
cp -vf debian/hh.dirs ../debian/hh.dirs
cp -vf debian/watch ../debian/watch

cd ../..
mv hh ${HH}
cd ${HH}

createTarball

debuild -us -uc
debuild -S

# eof
