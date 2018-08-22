#!/bin/bash
#
# Copyright (C) 2014-2018  Martin Dvorak <martin.dvorak@mindforger.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This script builds: upstream tarball > source deb > binary deb
#
# See:
#   Beginners guide:
#     http://packaging.ubuntu.com/html/packaging-new-software.html
#   Debian maintainers guide:
#     https://www.debian.org/doc/manuals/maint-guide/index.en.html
#     https://www.debian.org/doc/manuals/debmake-doc/index.en.html
#   Debian formal doc:
#     https://www.debian.org/doc/debian-policy/
#

# This script cannot be run from Git repository. Launchpad release
# directory must exist.

export HHSRC=/home/dvorka/p/hstr/github/hstr
export HHRELEASEDIR=/home/dvorka/p/hstr/launchpad
export SCRIPTHOME=`pwd`

# ############################################################################
# # Checkout HSTR from bazaar and make it #
# ############################################################################

function checkoutHh() {
    echo "Checking out HSTR from Bazaar to `pwd`"
    bzr checkout lp:~ultradvorka/+junk/hh-package
    cd hh-package && mv -v .bzr .. && rm -rvf *.* && mv -v ../.bzr .
    cp -rvf ${HHSRC}/* ${HHSRC}/*.*  .
    cd ..

    echo "Preparing *configure using Autotools"
    mv -v hh-package hstr
    cd ./hstr/build/tarball && ./tarball-automake.sh --purge && cd ../../..
}

# ############################################################################
# # Create updated changelog #
# ############################################################################

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

# ############################################################################
# # Create tar archive #
# ############################################################################

function createTarArchive() {
  cd ..
  mkdir work && cd work
  cp -vrf ../${HH} .
  rm -rvf ${HH}/.bzr
  tar zcf ../${HH}.tgz ${HH}
  cp -vf ../${HH}.tgz ../${HH}.orig.tar.gz
  cd ../${HH}
  rm -vrf ../work
}

# ############################################################################
# # Release for *ONE* particular Ubuntu version #
# ############################################################################

function releaseForParticularUbuntuVersion() {
    export UBUNTUVERSION=${1}
    export HHVERSION=${2}
    export HHBZRMSG=${3}

    export HHFULLVERSION=${HHVERSION}-0ubuntu1
    export HH=hh_${HHVERSION}
    export HHRELEASE=hh_${HHFULLVERSION}
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export HHBUILD=hstr-${NOW}
    
    # checkout HSTR from Bazaar and prepare *configure using Autotools
    mkdir ${HHBUILD} && cd ${HHBUILD}
    checkoutHh `pwd`

    # commit changes to Bazaar
    cd hstr
    cp -rvf ${HHSRC}/build/ubuntu/debian .
    createChangelog ./debian/changelog
    cd .. && mv hstr ${HH} && cd ${HH}
    bzr add .
    bzr commit -m "Update for ${HH} at ${NOW}."

    # create Tar archive
    createTarArchive

    # start GPG agent (if it's NOT running)
    if [ -e "${HOME}/.gnupg/S.gpg-agent" ]
    then
	echo "OK: GPG agent running."
    else
	gpg-agent --daemon
    fi    

    # build .debs
    # build UNSIGNED .deb package (us uc tells that no GPG signing is needed)
    bzr builddeb -- -us -uc
    # build SIGNED source .deb package
    bzr builddeb -S
    cd ../build-area
    
    # build binary from source deb on CLEAN system - no deps installed
    echo -e "\n_ hh pbuilder-dist build  _______________________________________________\n"
    # BEGIN: bug workaround - pbuilder's caches in /var and /home must be on same physical drive
    export PBUILDFOLDER=/tmp/hh-tmp
    rm -rvf ${PBUILDFOLDER}
    mkdir -p ${PBUILDFOLDER}
    cp -rvf ~/pbuilder/*.tgz ${PBUILDFOLDER}
    # END
    pbuilder-dist ${UBUNTUVERSION} build ${HHRELEASE}.dsc

    # push .deb to Launchpad
    cd ../${HH}
    # push Bazaar changes and upload .deb to Launchpad
    echo "Before bzr push: " `pwd`
    bzr push lp:~ultradvorka/+junk/hh-package
    cd ..
    echo "Before dput push: " `pwd`
    # recently added /ppa to fix the path and package rejections
    dput ppa:ultradvorka/ppa ${HHRELEASE}_source.changes
}

# ############################################################################
# # Main #
# ############################################################################

echo "This script is expected to be copied to and run from: ~/p/hstr/launchpad"

if [ -e "../../.git" ]
then
    echo "This script must NOT be run from Git repository - run it e.g. from ~/p/hstr/launchpad instead"
    exit 1
fi
if [ ! -e "${HHRELEASEDIR}" ]
then
    echo "ERROR: release directory must exist: ${HHRELEASEDIR}"
    exit 1
fi

export ARG_BAZAAR_MSG="Release 2.0"
export ARG_MAJOR_VERSION=2.0.
export ARG_MINOR_VERSION=0 # minor version is incremented for every Ubuntu version

# https://wiki.ubuntu.com/Releases
# old: precise quantal saucy precise utopic vivid wily yakkety artful
# current: trusty xenial bionic
for UBUNTU_VERSION in trusty xenial bionic
do
    echo "Releasing HSTR for Ubuntu version: ${UBUNTU_VERSION}"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} "${ARG_BAZAAR_MSG}"
    ARG_MINOR_VERSION=`expr $ARG_MINOR_VERSION + 1`
done

# eof
