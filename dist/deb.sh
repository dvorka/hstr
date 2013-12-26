#!/bin/bash

mv -vf ../debian ../..
./dist.sh
cd ../..
mv hstr ${HH}
tar zcf ${HH}.tgz ${HH}
bzr dh-make hh 0.7 ${HH}.tgz
cd hh
rm -rvf debian
cp -vrf ../debian .
bzr commit -m "Initial commit of Debian packaging."
bzr builddeb -- -us -uc
bzr builddeb -S
cd ../build-area
pbuilder-dist raring build ${HHRELEASE}.dsc

# eof
