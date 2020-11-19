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

# https://wiki.ubuntu.com/Releases

# boostrap new OR refresh distribution base for pbuilder
export DISTRO=groovy

sudo pbuilder --create $DISTRO
rm -vf ~/pbuilder/${DISTRO}-base.tgz
cp /var/cache/pbuilder/base.tgz ~/pbuilder/${DISTRO}-base.tgz

# eof
