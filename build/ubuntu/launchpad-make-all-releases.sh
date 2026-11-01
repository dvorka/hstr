#!/bin/bash
#
# Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>
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
# # Check dependencies #
# ############################################################################

function checkDependencies() {
    local missing_deps=()
    local missing_config=()

    # check for required commands
    if ! command -v bzr &> /dev/null; then
        missing_deps+=("bzr/brz")
    fi

    if ! command -v dput &> /dev/null; then
        missing_deps+=("dput")
    fi

    if ! command -v pbuilder-dist &> /dev/null; then
        missing_deps+=("ubuntu-dev-tools")
    fi

    if ! command -v debuild &> /dev/null; then
        missing_deps+=("devscripts")
    fi

    if ! dpkg -l debhelper &> /dev/null; then
        missing_deps+=("debhelper")
    fi

    # check for bzr-builddeb plugin (provided by brz-debian package as "debian" plugin)
    if command -v bzr &> /dev/null; then
        if ! bzr plugins 2>/dev/null | grep -q "debian"; then
            missing_deps+=("brz-debian")
        fi

        # check if bzr whoami is configured
        if ! bzr whoami &> /dev/null; then
            missing_config+=("bzr-whoami")
        fi
    fi

    # report missing dependencies
    if [ ${#missing_deps[@]} -gt 0 ]; then
        echo "ERROR: Missing required dependencies:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo ""
        echo "To install all required packages, run:"
        echo "  sudo apt-get install brz bzr brz-debian dput ubuntu-dev-tools devscripts debhelper pbuilder"
        exit 1
    fi

    # report missing configuration
    if [ ${#missing_config[@]} -gt 0 ]; then
        echo "ERROR: Missing required configuration:"
        for cfg in "${missing_config[@]}"; do
            case $cfg in
                bzr-whoami)
                    echo "  - Bazaar identity not set"
                    echo "    Run: bzr whoami \"Your Name <your.email@example.com>\""
                    ;;
            esac
        done
        exit 1
    fi

    # check for GPG key matching the debian maintainer email
    local bzr_email=$(bzr whoami 2>/dev/null | grep -oP '<\K[^>]+')
    if [ -n "$bzr_email" ]; then
        if ! gpg --list-secret-keys "$bzr_email" &> /dev/null; then
            missing_config+=("gpg-key")
        fi
    fi

    # report missing configuration (second check after GPG)
    if [ ${#missing_config[@]} -gt 0 ]; then
        echo "ERROR: Missing required configuration:"
        for cfg in "${missing_config[@]}"; do
            case $cfg in
                gpg-key)
                    echo "  - No GPG key found for: $bzr_email"
                    echo ""
                    echo "    OPTION 1: Generate new GPG key:"
                    echo "      gpg --full-generate-key"
                    echo "      (Use: Martin Dvorak <$bzr_email>)"
                    echo ""
                    echo "    OPTION 2: Import key from old machine:"
                    echo "      On old machine: gpg --export-secret-keys $bzr_email > ~/gpg-key.asc"
                    echo "      Transfer file securely, then: gpg --import ~/gpg-key.asc"
                    echo ""
                    echo "    After creating/importing key:"
                    echo "      - Upload to keyserver: gpg --keyserver keyserver.ubuntu.com --send-keys YOUR_KEY_ID"
                    echo "      - Add to Launchpad: https://launchpad.net/~ultradvorka/+editpgpkeys"
                    ;;
            esac
        done
        exit 1
    fi
}

# ############################################################################
# # Checkout HSTR from bazaar and make it #
# ############################################################################

function checkoutHstr() {
    echo "Checking out HSTR from Bazaar to `pwd`"
    # Create new branch hstr-package: bzr init && bzr push lp:~ultradvorka/+junk/hstr-package
    bzr checkout lp:~ultradvorka/+junk/hstr-package

    if [ ! -d "hstr-package" ]; then
        echo ""
        echo "ERROR: Bazaar checkout failed! This is usually due to:"
        echo "  1. SSH key not configured on Launchpad"
        echo "  2. Launchpad login not configured (run: bzr launchpad-login ultradvorka)"
        echo ""
        echo "To fix SSH authentication:"
        echo "  1. Generate SSH key: ssh-keygen -t rsa -b 4096"
        echo "  2. Add to Launchpad: https://launchpad.net/~ultradvorka/+editsshkeys"
        echo "  3. Configure bzr: bzr launchpad-login ultradvorka"
        echo ""
        exit 1
    fi

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
    # copy pbuilder base tarball if it exists in ~/pbuilder/
    if [ -f ~/pbuilder/${UBUNTUVERSION}-base.tgz ]; then
        cp -v ~/pbuilder/${UBUNTUVERSION}-base.tgz ${PBUILDFOLDER}/
    else
        echo "ERROR: pbuilder base tarball not found: ~/pbuilder/${UBUNTUVERSION}-base.tgz"
        echo "Please create it first by running:"
        echo "  pbuilder-dist ${UBUNTUVERSION} create"
        echo "  mkdir -p ~/pbuilder"
        echo "  sudo cp /var/cache/pbuilder/${UBUNTUVERSION}-base.tgz ~/pbuilder/"
        # NON critital error: exit 1
    fi
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

echo "IMPORTANT: make sure to login to Launchpad using 'bzr launchpad-login YOUR_LAUNCHPAD_ID' before running this script!"
echo "IMPORTANT: make sure your SSH key is configured on Launchpad: https://launchpad.net/~ultradvorka/+editsshkeys"
echo -e "This script is expected to be copied to and run from: ~/p/hstr/launchpad\n\n"

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

# check all required dependencies
checkDependencies

# version is parsed from hstr.c which is authoritative
export HSTR_VERSION=$(sed -n 's/.*hstr version \\"\([0-9][0-9.]*\)\\".*/\1/p' ${HSTRSRC}/src/hstr.c)
if [ -z "${HSTR_VERSION}" ]
then
    echo "ERROR: unable to parse HSTR version from ${HSTRSRC}/src/hstr.c"
    exit 1
fi
export ARG_MAJOR_VERSION="${HSTR_VERSION%.*}."
export ARG_BAZAAR_MSG="Release ${HSTR_VERSION%.*}"
export ARG_MINOR_VERSION=10 # minor version is incremented for every Ubuntu version

# https://en.wikipedia.org/wiki/Ubuntu_version_history
# https://wiki.ubuntu.com/Releases
# obsolete:
#   precise quantal saucy precise utopic vivid wily yakkety xenial artful cosmic disco eoan groovy hirsute impish oracular plucky
# missed:
#   oracular
# current :
#   trusty xenial bionic focal jammy noble . questing
# future:
#   resolute
# command :
#   trusty xenial bionic focal jammy noble questing
for UBUNTU_VERSION in trusty xenial bionic focal jammy plucky questing
do
    echo "Releasing HSTR for Ubuntu version: ${UBUNTU_VERSION}"
    releaseForParticularUbuntuVersion ${UBUNTU_VERSION} ${ARG_MAJOR_VERSION}${ARG_MINOR_VERSION} "${ARG_BAZAAR_MSG}"
    ARG_MINOR_VERSION=`expr $ARG_MINOR_VERSION + 1`
done

# eof
