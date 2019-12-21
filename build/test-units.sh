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

# Run unit tests: local + CI

# TODO calculate CORE count and use it below

# options
#export OPTION_RUN_VALGRIND=yes # run test(s) w/ Valgrind (comment this line to disable)

# generate test runner
./test-generate-unity-test-runner.sh

# compile
cd ../test && qmake hstr-unit-tests.pro && make clean && make -j 8
if [ ${?} -ne 0 ]
then
    exit 1
fi

# run
if [ ${OPTION_RUN_VALGRIND} ] 
then
    valgrind --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ./hstr-unit-tests
    # Valgrind's GDB
    #valgrind --vgdb=yes --vgdb-error=0 --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ./hstr-unit-tests
else
    ./hstr-unit-tests
fi

# eof
