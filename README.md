hstr
====

BASH History Suggest Box

DESCRIPTION
-----------
A command line utility that brings improved BASH command completion 
from the history. It aims to make completion easier and faster
than Ctrl-R.


[![Shell History Suggest Box](http://mindforger.com/projects/images/hh-1.jpg "Shell History Suggest Box")](http://mindforger.com/projects/images/hh-1.jpg)


UBUNTU 12.10/13.04/13.10 INSTALLATION
-------------------------
* install `hh` on Ubuntu:

    `sudo add-apt-repository ppa:ultradvorka/ppa`

    `sudo apt-get update`

    `sudo apt-get install hh`

* Configure `hh` as described below. 


DOWNLOAD
--------
https://github.com/dvorka/hstr/releases


SOURCE CODE INSTALLATION
------------------------
* download distribution (project Releases section), expand the archive and install `hh`:

    `./configure && make && make install`

* Configure `hh` as described below. 


CONFIGURATION
------------
* add 

     `shopt -s histappend`

     `export PROMPT_COMMAND="history -a; history -n; ${PROMPT_COMMAND}"`

  at the end of `.bashrc` in order to ensure that BASH history of commands 
  will be kept in sync with filesystem.
* bind `hh` to a BASH key by adding a line to `.bashrc`, e.g. Ctrl-R:

    `bind '"\C-r": "\C-k\C-uhh\C-j"'`

  or Ctrl-F12:

    `bind '"\e[24;5~":"\C-k\C-uhh\C-j"'`

  To determine the character sequence emitted by a pressed key in terminal, 
  type CTRL-v and then press the key. To clear the line first, add `\C-k \C-u` 
  in front of the actual command. Check your current bindings using:

    `bind -S`

BUGS
----
https://github.com/dvorka/hstr/issues

