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

# Debian release: https://wiki.debian.org/IntroDebianPackaging

export HHVERSION="1.25.0"
#export HHFULLVERSION=${HHVERSION}-1.0 # NMU upload
export HHFULLVERSION=${HHVERSION}-1    # mantainer upload
export HH=hstr_${HHVERSION}
export HHRELEASE=hh-${HHFULLVERSION}
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}

export UBUNTUVERSION=unstable

export HHBZRMSG="Debian distribution."

# eof
