hstr
====
BASH History Suggest Box.

[![BASH History Suggest Box](http://mindforger.com/projects/images/hh-3.png "BASH History Suggest Box @ YouTube")](http://www.youtube.com/watch?v=sPF29NyXe2U)


DESCRIPTION
-----------
Are you looking for a command that you used **recently**? Do you
want to  avoid the need to write **long commands** over and over
again despite you used them recently? Are you looking
for a tool that is able to manage your **favorite** commands?

HSTR is a command line utility that brings improved BASH command completion 
from the history. It aims to make completion **easier** and more **efficient**
than `Ctrl-r`.

Apart to the completion, history can be **managed** (you can remove 
commands that e.g. contain sensitive information like
passwords) and bookmark your favorite commands.

Enjoy [video](http://www.youtube.com/watch?v=sPF29NyXe2U) tutorial.

UBUNTU INSTALLATION
-------------------
* install `hh` on Ubuntu (12.04LTS/12.10/13.10/14.04LTS):
```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt-get update
sudo apt-get install hh
```

* optionally configure `hh`: 
```bash
hh --show-configuration >> ~/.bashrc
```

INSTALLATION FROM SOURCE CODE
-----------------------------
* Clone Git repository:
```bash
git clone https://github.com/dvorka/hstr.git
```

* create build files using:
```bash
cd ./dist && ./1-dist.sh && cd ..
```

* install `hh` using:
```bash
./configure && make && make install
```

* optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```


DISTRIBUTION INSTALLATION 
-------------------------
* download latest distribution from project Releases section:

https://github.com/dvorka/hstr/releases

* expand the archive and install `hh` using:
```bash
./configure && make && make install
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


ARCH LINUX
----------
* download latest distribution from project Releases section:

https://github.com/dvorka/hstr/releases

* use PKGBUILD in the root of the distribution to build package using `makepkg`:

https://wiki.archlinux.org/index.php/PKGBUILD

* install `hh`
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
show favorites by default (instead of metrics-based view):
```bash
export HH_CONFIG=favorites
```
make search case sensitive (insensitive by default):
```bash
export HH_CONFIG=casesensitive
```
show warnings:
```bash
export HH_CONFIG=warning
```
show debug messages:
```bash
export HH_CONFIG=debug
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
