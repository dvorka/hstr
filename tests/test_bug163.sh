#!/bin/bash

export HH_CONFIG=hicolor,regexp
export HISTFILE=/tmp/hh-bug-163

cp -vf ~/p/hstr/.bash_history_BUG_163 ${HISTFILE}

#cd ~/p/hstr/github/hstr/Debug && gdbserver :9999 ./hh
cd ~/p/hstr/github/hstr/Debug && ./hh
#hh

# eof
