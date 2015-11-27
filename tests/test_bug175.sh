#!/bin/bash

# switch term to forbid alternative page(s)
infocmp | sed -e 's/[sr]mcup=[^,]*,//' > /tmp/noaltscreen-terminfo
tic -o ~/.terminfo/ /tmp/noaltscreen-terminfo

#cd ~/p/hstr/github/hstr/Debug && gdbserver :9999 ./hh
#cd ~/p/hstr/github/hstr/Debug && ./hh
#hh
top

# eof
