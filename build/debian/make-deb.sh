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
#

# Method:
#   upstream tarball > source deb > binary deb
#
# See:
#   Beginners guide:
#     https://wiki.debian.org/IntroDebianPackaging
#   Debian maintainers guide:
#     https://www.debian.org/doc/manuals/maint-guide/index.en.html
#     https://www.debian.org/doc/manuals/debmake-doc/index.en.html
#   Debian formal doc:
#     https://www.debian.org/doc/debian-policy/
#

# ############################################################################
# # Create updated changelog #
# ############################################################################

function createChangelog() {
  # Debian tooling changelog hints: 
  # - signature line MUST have one whitespace prefix
  # - signature line MUST have double space between email and timestamp 
  # - traling lines must have exactly one space

  export MYTS=`date "+%a, %d %b %Y %H:%M:%S"`
  echo "Changelog timestamp: ${MYTS}"
  echo -e "hstr (${HSTRFULLVERSION}) ${DEBIANVERSION}; urgency=low" > $1
  echo " " >> $1
  echo -e "  * ${HSTRBZRMSG}" >> $1
  echo " " >> $1
  echo " -- Martin Dvorak (Dvorka) <martin.dvorak@mindforger.com>  ${MYTS} +0100" >> $1
}

# ############################################################################
# # Create upstream tarball #
# ############################################################################

function createTarball() {
  cd ..
  mkdir work
  cd work
  cp -vrf ../${HSTR} .
  tar zcf ../${HSTR}.tgz ${HSTR}
  # .orig.tar.gz is required Debian convention
  cp -vf ../${HSTR}.tgz ../${HSTR}.orig.tar.gz
  cd ../${HSTR}
}

# ############################################################################
# # Build source and binary deb packages #
# ############################################################################

function buildDebPackage() {
    export SCRIPTHOME=`pwd`
    export HSTRVERSION=$1
    export HSTRBZRMSG=$2
    #export HSTRFULLVERSION=${HSTRVERSION}-1.0 # NMU upload
    export HSTRFULLVERSION=${HSTRVERSION}-1    # mantainer upload
    export HSTR=hstr_${HSTRVERSION}
    export HSTRRELEASE=hstr-${HSTRFULLVERSION}
    export HSTRSRC=/home/dvorka/p/hstr/github/hstr
    export NOW=`date +%Y-%m-%d--%H-%M-%S`
    export HSTRBUILD=hstr-${NOW}
    export DEBIANVERSION=unstable
    
    #
    # 1) create upstream tarball
    #
    # 1.1) get copy of project source
    echo -e "\n# Get HSTR project files ############################"
    mkdir -p ${HSTRBUILD}/${HSTR}
    cd ${HSTRBUILD}/${HSTR}
    # copy  project files to current directory
    cp -rvf ${HSTRSRC}/* ${HSTRSRC}/*.*  .

    # 1.x) generate makefiles
    cd build/tarball && ./tarball-automake.sh && cd ../.. && ./configure
    
    # 1.2) prune HSTR project source: tests, *.o/... build files, ...
    echo -e "\n# HSTR project cleanup ########################################"
    rm -vrf ./.git ./hstr ./build ./test ./hstr.pro ./doc ./pad.xml ./snap
    find . -type f \( -name "*.a" -or -name "*.o" -or -name "*.*~" -or -name ".gitignore" -or -name ".git" \) | while read F; do rm -vf $F; done
        
    # 1.4) create tar archive
    createTarball

    #
    # 2) create source deb
    #    
    # 2.a) add Debian control files
    cp -rvf ${HSTRSRC}/build/debian/debian  .
    createChangelog ./debian/changelog

    # x) start GPG agent if it's NOT running
    if [ -e "${HOME}/.gnupg/S.gpg-agent" ]
    then
	echo "OK: GPG agent running."
    else
	gpg-agent --daemon
    fi    

    DEBEMAIL="martin.dvorak@mindforger.com"
    DEBFULLNAME="Martin Dvorak"
    export DEBEMAIL DEBFULLNAME

    # 2.b) build BINARY deb
    # https://www.debian.org/doc/manuals/maint-guide/build.en.html
    #debuild -us -uc
    dpkg-buildpackage -us -uc
    pushd .. && mkdir -v deb-binary && cp -vf *.dsc *.changes *.deb deb-binary && popd

    # 3) create source deb
    #    
    # 3.a) build deb
    # build SOURCE deb package
    #debuild -S
    dpkg-buildpackage -S
    pushd .. && mkdir -v deb-source && cp -vf *.dsc *.changes *.deb deb-source && popd
}

# ############################################################################
# # Main #
# ############################################################################

echo "This script MUST run on Debian OS (or VM)"
echo "This script is expected to be copied to and run from: ~/p/hstr/debian"

if [ -e "../.git" ]
then
  echo "This script must NOT be run from Git repository - run it e.g. from ~/p/hstr/debian instead"
  exit 1
fi

export ARG_BAZAAR_MSG="HSTR 2.3.0"
export ARG_VERSION="2.3.0"

# Debian releases: https://www.debian.org/releases/
#   6/7/8/9/10: squeeze wheezy jessie stretch buster

buildDebPackage ${ARG_VERSION} ${ARG_BAZAAR_MSG}

# eof
