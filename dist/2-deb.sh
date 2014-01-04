#!/bin/bash

export SCRIPTHOME=`pwd`

. ./env.sh

rm -rvf ../debian
cp -rvf ${HHSRC}/debian ..

cp changelog-${UBUNTUVERSION} ../debian/changelog
cd ..
dch -i
cd dist

cd ../..
mv hh ${HH}
cd ${HH}
bzr commit -m "Update for ${HH} at ${NOW}."
bzr builddeb -- -us -uc
bzr builddeb -S
cd ../build-area
pbuilder-dist ${UBUNTUVERSION} build ${HHRELEASE}.dsc

# eof
