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

rm -vrf ../.git ../.settings ../Debug ../Release ../.cproject ../.project ../.gitignore

rm -vf *.*~ ../*.*~ ../src/*.*~
rm -rvf ../atom5te.cache
rm -vf ../man/Makefile ../man/Makefile.in
rm -vf ../src/Makefile ../src/Makefile.in ../src/hh ../src/*.o 
rm -rvf ../src/.deps
rm -vf ../Makefile ../Makefile.in ../aclocal.m4 ../config.log ../config.status ../configure ../depcomp ../install-sh ../missing

# eof
