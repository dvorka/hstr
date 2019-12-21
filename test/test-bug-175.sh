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

# switch term to forbid alternative page(s)
infocmp | sed -e 's/[sr]mcup=[^,]*,//' > /tmp/noaltscreen-terminfo
tic -o ~/.terminfo/ /tmp/noaltscreen-terminfo

#cd ~/p/hstr/github/hstr && gdbserver :9999 ./hstr
#cd ~/p/hstr/github/hstr && ./hstr
top

# eof
