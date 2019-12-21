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

# Run HSTR w/ 1 history entry to hunt memleaks w/ valgrind

#export OPT_ALL_SCENARIOS=true

# both valid and INVALID history file to be tested
#export HISTFILE=
export HISTFILE="/tmp/invalid-history-file-01"
#export HISTFILE="~/.bash_history"
#export HISTFILE="~/.zhistory"
#export HISTFILE="~/.zshrc"

# build
cd .. && qmake CONFIG+=hstrdebug hstr.pro && make clean && make -j 8
if [ ${?} -ne 0 ]
then
    exit 1
fi

# test scenarios
export FILE_SCENARIOS="/tmp/hstr-scenarios.txt"
if [ ${OPT_ALL_SCENARIOS} ]
then
    echo "./hstr --help" > ${FILE_SCENARIOS}
    echo "./hstr --version" >> ${FILE_SCENARIOS}
    echo "./hstr --kill-last-command" > ${FILE_SCENARIOS}
    echo "./hstr --show-configuration" >> ${FILE_SCENARIOS}
    echo "./hstr --show-zsh-configuration" >> ${FILE_SCENARIOS}
    echo "./hstr --show-blacklist" >> ${FILE_SCENARIOS}
    echo "./hstr --non-interactive echo" >> ${FILE_SCENARIOS}
    echo "./hstr -n log" >> ${FILE_SCENARIOS}
else
    echo "./hstr -k" > ${FILE_SCENARIOS}
fi	 
# ELSE following scenarios must be run MANUALLY from CLI
# valgrind --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ./hstr
#   ENTER
#   Ctrl-g
#   a > ENTER
#   a > right arrow
# valgrind --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ./hstr log
#   ENTER
#   Ctrl-g
#   right arrow



# test history file - comment ALL HISTFILE exports below for test w/ production
#export HISTFILE=`pwd`/../test/resources/.bash_history_valgrind_empty
#export HISTFILE=`pwd`/../test/resources/.bash_history_valgrind_1_entry

# run tests w/ Valgrind
cat ${FILE_SCENARIOS} | while read SCENARIO
do
    # Valgrind
    valgrind --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ${SCENARIO}

    # Valgrind's GDB
    #valgrind --vgdb=yes --vgdb-error=0 --track-origins=yes --tool=memcheck --leak-check=full --show-leak-kinds=all ${SCENARIO}
done

# eof
