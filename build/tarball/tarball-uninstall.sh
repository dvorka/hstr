#!/bin/bash
#
# Copyright (C) 2014-2021  Martin Dvorak <martin.dvorak@mindforger.com>
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

# Run this script as root to uninstall HSTR installed using autotools

export ME=`whoami`
if [ ! "root" == "${ME}" ]
then
    echo "ERROR: run this script as root"
    exit 1
fi

rm -vf /usr/local/bin/hh
rm -vf /usr/local/bin/hstr
rm -vf /usr/share/bash-completion/completions/hh
rm -vf /usr/share/bash-completion/completions/hstr

# eof
