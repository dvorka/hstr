#!/bin/bash

export HH_CONFIG=hicolor
export HISTFILE=/tmp/hh-bug-124

cp -vf ~/p/hstr/.bash_history_BUG_124 ${HISTFILE}

#cd ~/p/hstr/github/hstr/Debug && gdbserver :9999 ./hh
#cd ~/p/hstr/github/hstr/Debug && ./hh
hh

# eof
