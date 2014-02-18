hstr
====

BASH History Suggest Box - [http://www.youtube.com/watch?v=sPF29NyXe2U](http://www.youtube.com/watch?v=sPF29NyXe2U)


DESCRIPTION
-----------
A command line utility that brings improved BASH command completion 
from the history. It aims to make completion easier and more efficient
than `Ctrl-r`.

[![BASH History Suggest Box](http://mindforger.com/projects/images/hh-2.jpg "BASH History Suggest Box")](http://mindforger.com/projects/images/hh-2.jpg)


UBUNTU INSTALLATION
-------------------
* install `hh` on Ubuntu (12.10/13.10):
```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt-get update
sudo apt-get install hh
```

* optionally configure `hh`: 
```bash
hh --show-configuration >> ~/.bashrc
```


DEBIAN/MINT INSTALLATION
-------------------
* install `hh` on Debian (Wheezy) or Mint (13) by registering the key:
```bash
wget www.clfh.de/frankh.asc
apt-key add frankh.asc
```
adding the repository:
```bash
deb http://www.clfh.de/debian wheezy main
deb-src http://www.clfh.de/debian wheezy main
```
and installing it:
```bash
apt-get update
apt-get install hh
```

* optionally configure `hh`: 
```bash
hh --show-configuration >> ~/.bashrc
```


DOWNLOAD
--------
https://github.com/dvorka/hstr/releases


SOURCE CODE INSTALLATION
------------------------
* download distribution (project Releases section), expand the archive and install `hh`:
```bash
./configure && make && make install
```

* optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```


MAC OS
------
* install MacPorts:

   http://www.macports.org

* set the environment:
```bash
env CFLAGS="-I/opt/local/include -I/usr/include" LDFLAGS="-L/opt/local/lib -L/usr/lib" ./configure
```

* finish the installation
```bash
make && make install
```

* optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```


CONFIGURATION EXPLANATION
-------------------------
* `hh` works best with the optional configuration described in this section.
  You can configure `hh` just by running:
```bash
hh --show-configuration >> ~/.bashrc
```

* bind `hh` to a BASH key e.g. to `Ctrl-r`:
```bash
bind '"\C-r": "\C-ahh \C-j"'
```
or `Ctrl-Alt-r`:
```bash
bind '"\e\C-r":"\C-ahh \C-j"'
```
or `Ctrl-F12`:
```bash
bind '"\e[24;5~":"\C-ahh \C-j"'
```
bind `hh` to `Ctrl-r` only if this is interactive shell:
```bash
if [[ $- =~ .*i.* ]]; then bind '"\C-r": "\C-a hh \C-j"'; fi
```

To determine the character sequence emitted by a pressed key in terminal, 
type `Ctrl-v` and then press the key. Check your current bindings using:
```bash
bind -S
```

* get `hh` in more colors:
```bash
export HH_CONFIG=hicolor
```
show normal history by default (instead of metrics-based view):
```bash
export HH_CONFIG=rawhistory
```
make search case sensitive (insensitive by default):
```bash
export HH_CONFIG=casesensitive
```
more colors and case sensitive search:
```bash
export HH_CONFIG=hicolor,casesensitive
```

* increase the size of history:
```bash
export HISTFILESIZE=10000
export HISTSIZE=${HISTFILESIZE}
```
Variables defined above increase the number of history items and history file size
(default value is 500).

* ensure syncing (flushing and reloading) of `.bash_history` with in-memory 
  history:
```bash
export PROMPT_COMMAND="history -a; history -n; ${PROMPT_COMMAND}"
```

* force appending of in-memory history to `.bash_history` 
  (instead of overwriting): 
```bash
shopt -s histappend
```

* use leading space to hide commands from history:
```bash
export HISTCONTROL=ignorespace
```
Suitable for a sensitive information like passwords.


BUGS
----
https://github.com/dvorka/hstr/issues
