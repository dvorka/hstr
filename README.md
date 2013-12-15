hstr
====

BASH History Suggest Box

DESCRIPTION
-----------
A command line utility that brings improved BASH command completion 
from the history. It aims to make completion easier and faster
than Ctrl-R.


DOWNLOAD
--------
https://github.com/dvorka/hstr/releases


INSTALLATION
------------
* bind `hh` to a key and/or add it to $PATH
* bind `hh` to a BASH key by adding a line to your `.bashrc`, e.g. Ctrl-R:

    `bind '"\C-r": "\C-k\C-uhh\C-j"'`

  or F12:

    `bind '"\e[24~":"\C-k\C-uhh\C-j"'` 

  or Ctrl-F12:

    `bind '"\e[24;5~":"\C-k\C-uhh\C-j"'`

  To determine the character sequence emitted by a pressed key in terminal, 
  type CTRL-v and then press the key. For example, F12 gives `^[[24~`. 
  Replace the `^[` with `\e`. To clear the line first, add `\C-k \C-u` in front of 
  the actual command. Check your current bindings using:

    `bind -S`

BUGS
----
https://github.com/dvorka/hstr/issues

