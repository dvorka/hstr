#!/bin/bash

export SCRIPTHOME=`pwd`

. ./debian-env.sh

function createChangelog() {
  # Debian tooling changelog hints: 
  # - signature line MUST have one whitespace prefix
  # - signature line MUST have double space between email and timestamp
  # - traling lines must have exactly one space

  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo -e "hstr (${HHFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo " " >> $1
  echo -e "  * ${HHBZRMSG}" >> $1
  echo " " >> $1
  echo " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
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

echo -e "\n_ HSTR deb build  _______________________________________________\n"

rm -rvf ../debian
cp -rvf ${HHSRC}/debian ..

createChangelog ../debian/changelog
cp -vf debian/rules ../debian/rules
cp -vf debian/hstr.dirs ../debian/hstr.dirs
cp -vf debian/watch ../debian/watch

cd ../..
mv hstr ${HH}
cd ${HH}

createTarball

debuild -us -uc
debuild -S

# eof
