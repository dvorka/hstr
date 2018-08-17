#!/bin/bash

export HH_CONFIG=hicolor
export HISTFILE=/tmp/hh-bug-176

cp -vf ~/p/hstr/github/hstr/tests/resources/.bash_history_BUG_176 ${HISTFILE}

#cd ~/p/hstr/github/hstr/Debug && gdbserver :9999 ./hh
#cd ~/p/hstr/github/hstr/Debug && ./hh
hh

# eof
