#!/bin/bash

# copy hstr/ source code to a work directory and run this script to create DEB package
mv -vf ../debian ../..
./dist.sh
cd ../..
mv hstr hh-0.7
tar zcf hh-0.7.tgz hh-0.7
bzr dh-make hh 0.7 hh-0.7.tgz
cd hh
rm -rvf debian
cp -vrf ../debian .
bzr commit -m "Initial commit of Debian packaging."
bzr builddeb -- -us -uc

bzr builddeb -S && cd ../build-area && pbuilder-dist raring build hh_0.7-0ubuntu1.dsc

# eof
