hstr
====

BASH History Suggest Box

DESCRIPTION
-----------
A command line utility that brings improved BASH command completion 
from the history. It aims to make completion easier and faster
than Ctrl-R.


[![Shell History Suggest Box](http://mindforger.com/projects/images/hh-1.jpg "Shell History Suggest Box")](http://mindforger.com/projects/images/hh-1.jpg)


UBUNTU INSTALLATION
-------------------
* install `hh` on Ubuntu (12.10/13.04/13.10):

    `sudo add-apt-repository ppa:ultradvorka/ppa`

    `sudo apt-get update`

    `sudo apt-get install hh`

* Configure `hh`: 

      `hh --show-configuration >> ~/.bashrc`


DOWNLOAD
--------
https://github.com/dvorka/hstr/releases


SOURCE CODE INSTALLATION
------------------------
* download distribution (project Releases section), expand the archive and install `hh`:

    `./configure && make && make install`

* Configure `hh`:

      `hh --show-configuration >> ~/.bashrc`


MAC OS
------
* install MacPorts:

   http://www.macports.org

* configure using:

   `env CFLAGS="-I/opt/local/include -I/usr/include" LDFLAGS="-L/opt/local/lib -L/usr/lib" ./configure`

* finish the installation

    `make && make install`

* Configure `hh`:

      `hh --show-configuration >> ~/.bashrc`


CONFIGURATION EXPLANATION
-------------------------
* appending of the in memory history (instead of overwriting it) 
  to `.bash_history`: 

    `shopt -s histappend`

* flushing and realoading of `.bash_history` in order to ensure that
  it is in sync with memory:

     `export PROMPT_COMMAND="history -a; history -n; ${PROMPT_COMMAND}"`

* binding of `hh` to a BASH key (e.g. to `Ctrl-R`):

    `bind '"\C-r": "\C-ahh \C-j"'`

  `Ctrl-F12` would be:

    `bind '"\e[24;5~":"\C-ahh \C-j"'`

  To determine the character sequence emitted by a pressed key in terminal, 
  type CTRL-v and then press the key. Check your current bindings using:

    `bind -S`

BUGS
----
https://github.com/dvorka/hstr/issues

