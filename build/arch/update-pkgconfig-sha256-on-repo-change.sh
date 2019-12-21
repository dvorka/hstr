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

# This script should be made Git hook to update Sha256 in ArchLinux PKGBUILD
# See also: https://github.com/dvorka/hstr/issues/304

fixPkgbuildSha256() {
    cd ../..

    if [ ! -f ./PKGBUILD ]; then
	echo "No PKGBUILD in `pwd`"
	return 1
    fi

    local sums
    # calculate new md5sums
    sums=$(makepkg -g)
    # replace them in-place
    sed -i "s/^md5sums=.*/%NEWSUMS%/;/'[a-z0-9]\{32\}'/d;s/%NEWSUMS%/$sums/" PKGBUILD
    echo "PKGBUILD updated"
}

fixPkgbuildSha256

# eof
