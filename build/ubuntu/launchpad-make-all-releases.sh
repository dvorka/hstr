#!/bin/bash
#
# Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>
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

export HSTRSRC=/home/dvorka/p/hstr/github/hstr
export HSTRRELEASEDIR=/home/dvorka/p/hstr/launchpad
export SCRIPTHOME=`pwd`

# ############################################################################
# # Checkout HSTR from bazaar and make it #
# ############################################################################

function checkoutHstr() {
    echo "Checking out HSTR from Bazaar to `pwd`"
    # Create new branch hstr-package: bzr init && bzr push lp:~ultradvorka/+junk/hstr-package
    bzr checkout lp:~ultradvorka/+junk/hstr-package
    cd hstr-package && mv -v .bzr .. && rm -rvf *.* && mv -v ../.bzr .
    cp -rvf ${HSTRSRC}/* ${HSTRSRC}/*.*  .
    cd ..

    echo "Preparing *configure using Autotools"
    mv -v hstr-package hstr
    cd ./hstr/build/tarball && ./tarball-automake.sh --purge && cd ../../..
}

# ############################################################################
# # Create updated changelog #
# ############################################################################

function createChangelog() {
    export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
    echo "Changelog timestamp: ${MYTS}"
    echo -e "hstr (${HSTRFULLVERSION}) ${UBUNTUVERSION}; urgency=low" > $1
    echo -e "\n" >> $1
    echo -e "  * ${HSTRBZRMSG}" >> $1
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
  cp -vrf ../${HSTR} .
  rm -rvf ${HSTR}/.bzr
  tar zcf ../${HSTR}.tgz ${HSTR}
  cp -vf ../${HSTR}.tgz ../${HSTR}.orig.tar.gz
  cd ../${HSTR}
  rm -vrf ../work
}

# ############################################################################
# # Release for *ONE* particular Ubuntu version #
# ############################################################################

function releaseForParticularUbuntuVersion() {
    export UBUNTUVERSION=${1}
    export HSTRVERSION=${2}
    export HSTRBZRMSG=${3}

    export HSTRFULLVERSION=${HSTRVERSION}-0ubuntu1
    export HSTR=hstr_${HSTRVERSION}
    export HSTRRELEASE=hstr_${HSTRFULLVERSION}
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export HSTRBUILD=hstr-${NOW}
    
    # checkout HSTR from Bazaar and prepare *configure using Autotools
    mkdir ${HSTRBUILD} && cd ${HSTRBUILD}
    checkoutHstr `pwd`

    # commit changes to Bazaar
    cd hstr
    cp -rvf ${HSTRSRC}/build/ubuntu/debian .
    createChangelog ./debian/changelog
    cd .. && mv hstr ${HSTR} && cd ${HSTR}
    bzr add .
    bzr commit -m "Update for ${HSTR} at ${NOW}."

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
    # OPTIONAL test build w/o signing: build UNSIGNED .deb package (us uc tells that no GPG signing is needed)
    #bzr builddeb -- -us -uc
    # build SIGNED source .deb package
    bzr builddeb -S
    cd ../build-area
    
    # build binary from source deb on CLEAN system - no deps installed
    echo -e "\n_ hstr pbuilder-dist build  _______________________________________________\n"
    # BEGIN: bug workaround - pbuilder's caches in /var and /home must be on same physical drive
    export PBUILDFOLDER=/tmp/hstr-tmp
    rm -rvf ${PBUILDFOLDER}
    mkdir -p ${PBUILDFOLDER}
    cp -rvf ~/pbuilder/*.tgz ${PBUILDFOLDER}
    # END
    pbuilder-dist ${UBUNTUVERSION} build ${HSTRRELEASE}.dsc

    # push .deb to Launchpad
    cd ../${HSTR}
    # push Bazaar changes and upload .deb to Launchpad
    echo "Before bzr push: " `pwd`
    bzr push lp:~ultradvorka/+junk/hstr-package
    cd ..
    echo "Before dput push: " `pwd`
    # recently added /ppa to fix the path and package rejections
    dput ppa:ultradvorka/ppa ${HSTRRELEASE}_source.changes
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
if [ ! -e "${HSTRRELEASEDIR}" ]
then
    echo "ERROR: release directory must exist: ${HSTRRELEASEDIR}"
    exit 1
fi

export ARG_BAZAAR_MSG="Release 2.3"
export ARG_MAJOR_VERSION=2.3.
export ARG_MINOR_VERSION=0 # minor version is incremented for every Ubuntu version

# https://wiki.ubuntu.com/Releases
# old: precise quantal saucy precise utopic vivid wily yakkety artful cosmic
# current: trusty xenial bionic focal groovy
for UBUNTU_VERSION in trusty xenial bionic focal groovy
do
    echo "Releasing HSTR for Ubuntu version: ${UBUNTU_VERSION}"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} "${ARG_BAZAAR_MSG}"
    ARG_MINOR_VERSION=`expr $ARG_MINOR_VERSION + 1`
done

# eof
