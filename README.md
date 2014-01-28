hstr
====

BASH History Suggest Box - [http://www.youtube.com/watch?v=sPF29NyXe2U](http://www.youtube.com/watch?v=sPF29NyXe2U)


DESCRIPTION
-----------
A command line utility that brings improved BASH command completion 
from the history. It aims to make completion easier and more efficient
than `Ctrl-r`.

[![Shell History Suggest Box](http://mindforger.com/projects/images/hh-1.jpg "Shell History Suggest Box")](http://mindforger.com/projects/images/hh-1.jpg)


UBUNTU INSTALLATION
-------------------
* install `hh` on Ubuntu (12.10/13.04/13.10):
```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt-get update
sudo apt-get install hh
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

* bind `hh` to a BASH key e.g. to `Ctrl-R`:
```bash
bind '"\C-r": "\C-ahh \C-j"'
```
or `Ctrl-F12`:
```bash
bind '"\e[24;5~":"\C-ahh \C-j"'
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
