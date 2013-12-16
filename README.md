hstr
====

BASH History Suggest Box

DESCRIPTION
-----------
A command line utility that brings improved BASH command completion 
from the history. It aims to make completion easier and faster
than Ctrl-R.


[![Shell History Suggest Box](http://www.mindcognition.com/hstr/hstr.png "Shell History Suggest Box")](https://github.com/dvorka/hstr)


DOWNLOAD
--------
https://github.com/dvorka/hstr/releases


INSTALLATION
------------
* add 

     `shopt -s histappend`

     `export PROMPT_COMMAND="history -a; history -n; ${PROMPT_COMMAND}"`

  at the end of `.bashrc` in order to ensure that BASH history of commands 
  will be kept in sync with filesystem.
* bind `hh` to a BASH key by adding a line to `.bashrc`, e.g. Ctrl-R:

    `bind '"\C-r": "\C-k\C-uhh\C-j"'`

  or F12:

    `bind '"\e[24~":"\C-k\C-uhh\C-j"'` 

  or Ctrl-F12:

    `bind '"\e[24;5~":"\C-k\C-uhh\C-j"'`

  To determine the character sequence emitted by a pressed key in terminal, 
  type CTRL-v and then press the key. For example, F12 gives `^[[24~`. 
  Replace the `^[` with `\e`. To clear the line first, add `\C-k \C-u` in 
  front of the actual command. Check your current bindings using:

    `bind -S`

BUGS
----
https://github.com/dvorka/hstr/issues

