#!/bin/bash
#
# Copyright (C) 2014-2020 Martin Dvorak <martin.dvorak@mindforger.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Method: binary deb -alien-> -fixes-> rpm

# This script is available from http://www.mindforger.com/fedora/fedora-rpm-from-deb.sh
# to be easily available in VMs

export MFVERSION="2.3.0"
export MFPRJNAME="hstr-${MFVERSION}"
export AMD64NAME="hstr_${MFVERSION}-1_amd64"
export I386NAME="hstr_${MFVERSION}-1_i386"
export NOARCHNAME="hstr_${MFVERSION}-1_noarch"

echo "IMPORTANT: this script must be run as root!"

function decompose() {
    # cleanup
    rm -rvf ${MFPRJNAME} *.rpm

    # a) everything works just fine
    #alien -r hstr_0.7.1-1_amd64.deb

    # b) alien generates RPM that conflicts w/ other RPMs
    #    https://www.electricmonk.nl/log/2017/02/23/how-to-solve-rpms-created-by-alien-having-file-conflicts/
    alien -r -g -v "${AMD64NAME}.deb"

    # remove the following lines:
    #%dir "/"
    #%dir "/usr/bin/"
    sed -i 's#%dir "/"##' ${MFPRJNAME}/${MFPRJNAME}-2.spec
    sed -i 's#%dir "/usr/bin/"##' ${MFPRJNAME}/${MFPRJNAME}-2.spec
    sed -i 's#%dir "/usr/lib/"##' ${MFPRJNAME}/${MFPRJNAME}-2.spec
}

function compose() {
    # recreate RPM
    cd ${MFPRJNAME}
    export MFRPMROOT=`pwd` 
   
    # bin build
    # --target=x86_64 
    # --target=i386
    rpmbuild --target=x86_64 --buildroot ${MFRPMROOT}/ -bb ${MFPRJNAME}-2.spec
    # noarch would be for SOURCE deb
    #rpmbuild --target=noarch --buildroot ${MFRPMROOT}/ -bb ${MFPRJNAME}-2.spec
}

######################################################################################

# Steps:
# 1. run decompose
# 2. build HSTR manually (dependencies like libncursesw5 will be replaced w/ libncursesw6, etc.)
#    build/tarball/tarball-automake.sh > cd .. > ./configure > make
# 3. replace `hstr` binary in decomposed package ... so that compose can infer lib dependencies
#    supported on fedure
# 4. run compose
# 5. install rpm to verify package
#    sudo dnf install hstr-...rpm

#decompose
# build HSTR
# compose

# end
