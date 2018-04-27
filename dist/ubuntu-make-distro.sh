#!/bin/bash
# ubuntu-make-distro.sh [ubuntu version] [hh version] [change description]
#   - ./ubuntu-make-distro.sh raring 1.9.5 "Fixed #25."
#   - this script to be run from ./launchpad or other dir that allows:
#     ../github/hstr
#

export UBUNTUVERSION=$1
## https://wiki.ubuntu.com/Releases
#export UBUNTUVERSION=precise
#export UBUNTUVERSION=quantal
#export UBUNTUVERSION=saucy
#export UBUNTUVERSION=trusty
#export UBUNTUVERSION=utopic
#export UBUNTUVERSION=xenial
#export UBUNTUVERSION=yakkety
export HHVERSION=$2
export HHBZRMSG=$3

export HHFULLVERSION=${HHVERSION}-0ubuntu1
export HH=hh_${HHVERSION}
export HHRELEASE=hh_${HHFULLVERSION}
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}

# checkout hh from bazaar and make hstr ################################

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

cd hh/dist

# build .deb for Ubuntu #############################################################
# ./2-ubuntu-build-deb.sh

export SCRIPTHOME=`pwd`

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

cd ../..
mv hh ${HH}
cd ${HH}
bzr add .
bzr commit -m "Update for ${HH} at ${NOW}."

createTarball

    # x) start GPG agent if it's NOT running
    if [ -e "${HOME}/.gnupg/S.gpg-agent" ]
    then
	echo "OK: GPG agent running."
    else
	gpg-agent --daemon
    fi    

bzr builddeb -- -us -uc
bzr builddeb -S
cd ../build-area


echo -e "\n_ hh pbuilder-dist build  _______________________________________________\n"
# BEGIN: bug workaround - pbuilder's caches in /var and /home must be on same physical drive
export PBUILDFOLDER=/tmp/hh-tmp
rm -rvf ${PBUILDFOLDER}
mkdir -p ${PBUILDFOLDER}
cp -rvf ~/pbuilder/*.tgz ${PBUILDFOLDER}
# END
pbuilder-dist ${UBUNTUVERSION} build ${HHRELEASE}.dsc

# push .deb to Launchpad ########################################################

# from buildarea/ to ./dist
cd ../${HH}

echo "Before bzr push: " `pwd`
bzr push lp:~ultradvorka/+junk/hh-package
cd ..
echo "Before dput push: " `pwd`
# recently added /ppa to fix the path and package rejections
dput ppa:ultradvorka/ppa ${HHRELEASE}_source.changes

# eof
