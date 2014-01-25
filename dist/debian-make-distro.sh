#!/bin/bash

. /home/dvorka/p/hstr/github/hstr/dist/debian-env.sh

rm -rvf *.*~ ./debian

mkdir ${HHBUILD}
cd ${HHBUILD}

cp -rvf ${HHSRC} .
cd ./hstr/dist && ./1-dist.sh && rm -vrf ../tests && cd ../..
cd `pwd`
mv hstr ${HH}
tar zcf ${HH}.tgz ${HH}
rm -rvf ${HH}
bzr dh-make -v hh ${HHVERSION} ${HH}.tgz
rm -vf *.orig.tar.gz

cd hh/dist
./2-debian-build-deb.sh

pwd
echo -e "\n\nFinish by running 3-debian-push-mentors.sh\n"

# eof
