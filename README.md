hstr
====

BASH History Suggest Box.

DESCRIPTION
-----------
A command line utility that brings improved BASH command completion 
from the history. It aims to make completion easier to use and faster
than Ctrl-R.


DOWNLOAD
--------
https://bitbucket.org/dvorka/hstr/downloads  


INSTALLATION
------------
* add hh to $PATH
* you may also bind hh to a BASH key (e.g. F12) by adding bind to .bashrc:
      bind '"\e[24~":"hh"' 
    To determine the character sequence emitted by a pressed key in terminal, 
    type CTRL-v and then press the key. For example, F12 gives "^[[24~" (no quotes). 
    Replace the "^[" with \e. To clear the line first, add “\C-k \C-u” in front of 
    the actual command to clear the line first. 
      bind '"\e[24~":"\C-kX\C-uhh\n"'
    Binding using Ctrl-F12:
      bind '"\e[24;5~":"\C-kX\C-uhh\n"'
    Check:
      bind -S


BUGS
----
https://bitbucket.org/dvorka/hstr/issues/new  


AUTHOR
------
martin.dvorak@mindforger.com
  
- eof -
