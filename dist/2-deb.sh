#!/bin/bash

export SCRIPTHOME=`pwd`

. ./env.sh

function createChangelog() {
  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo -e "hh (${HHFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
  echo -e "\n" >> $1
  echo -e "  * ${HHBZRMSG}" >> $1
  echo -e "\n" >> $1
  echo -e " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
  echo -e "\n" >> $1
}

rm -rvf ../debian
cp -rvf ${HHSRC}/debian ..

createChangelog ../debian/changelog

cd ../..
mv hh ${HH}
cd ${HH}
bzr commit -m "Update for ${HH} at ${NOW}."
bzr builddeb -- -us -uc
bzr builddeb -S
cd ../build-area
pbuilder-dist ${UBUNTUVERSION} build ${HHRELEASE}.dsc

# eof
