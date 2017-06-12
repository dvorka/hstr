INSTALLATION
============
Install `hh` using [distribution](#distribution-installation),
from [source code](#installation-from-source-code), on [Ubuntu](#ubuntu),
[Fedora/RHEL/CentOS/ScientificLinux](#fedorarhelcentos), [openSUSE](#opensuse), [Debian/Mint](#debianmint),
[Gentoo](#gentoo), [Arch Linux](#arch-linux), [Solus OS](#solus-os), [Bash on Ubuntu on Windows (WLS)](#wls), [Cygwin](#cygwin) or [Mac OS](#mac-os).


UBUNTU
------
Install `hh` on Ubuntu - one-liner:
```bash
sudo add-apt-repository ppa:ultradvorka/ppa && sudo apt-get update && sudo apt-get install hh && hh --show-configuration >> ~/.bashrc
```
... or step by step:
```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt-get update
sudo apt-get install hh
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).


FEDORA/RHEL/CENTOS
------------------
Install `hh` on Fedora, RHEL or CentOS:

```bash
sudo dnf|yum install hstr -y
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).


openSUSE
--------
For openSUSE Leap 42.1 run the following as root:

```bash
zypper addrepo http://download.opensuse.org/repositories/home:tuw-e184/openSUSE_Leap_42.1/home:tuw-e184.repo
zypper refresh
zypper install hstr
```


INSTALLATION FROM SOURCE CODE
-----------------------------
Clone Git repository:
```bash
git clone https://github.com/dvorka/hstr.git
```
Create build files using:
```bash
cd ./dist && ./1-dist.sh && cd ..
```
Install `hh` using:
```bash
./configure && make && make install
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).


DISTRIBUTION INSTALLATION
-------------------------
Download latest distribution from project Releases section:

https://github.com/dvorka/hstr/releases

Expand the archive and install `hh` using:
```bash
./configure && make && make install
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).


DEBIAN/MINT
-----------
Install `hh` on Debian (Wheezy) or Mint (13) by registering the key:
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
Optionally configure `hh`:
```bash
hh --show-configuration >> ~/.bashrc
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).


GENTOO
------
hstr has been added to Portage, see this [bug](https://bugs.gentoo.org/show_bug.cgi?id=527122).
To install hstr, emerge it like so:
```
emerge app-shells/hstr
```
and you should be all set.


ARCH LINUX
----------
Download latest distribution from project Releases section:

https://github.com/dvorka/hstr/releases

Use [PKGBUILD](https://wiki.archlinux.org/index.php/PKGBUILD) in the root of the distribution to build package using `makepkg`:

https://aur.archlinux.org/packages/hstr-git/

Install `hh`.

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

SOLUS OS
-------
Make sure you updated your system.
```bash
eopkg update-repo
```

Install `hh`

```bash
eopkg install hh
```

MAC OS
------
You can either install `hh` from [Homebrew](http://brew.sh/):

```bash
brew install hh
```

or from the source code as described further in this section. First install MacPorts:

   http://www.macports.org

And then install readline and ncurses:

```
port install readline
port install ncurses
```

Build and install (if using Homebrew):
```bash
autoreconf -fvi
./configure CFLAGS=-I$(brew --prefix)/opt/readline/include LDFLAGS=-L$(brew --prefix)/opt/readline/lib
make
```

Alternative if using MacPorts ([macports.com](https://www.macports.org/ports.php?by=name&substr=hstr)/[GitHub](https://github.com/macports/macports-ports/blob/master/shells/hstr/Portfile)):
```bash
autoreconf -fvi
./configure CFLAGS=-I/opt/local/include/ LDFLAGS=-L/opt/local/lib
make
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).


WLS
---
To install `hh` on [Bash on Ubuntu on Windows (WLS)](https://msdn.microsoft.com/en-us/commandline/wsl/about) clone Git repository:
```bash
git clone https://github.com/dvorka/hstr.git
```
Create build files using:
```bash
cd ./dist && ./1-dist.sh && cd ..
```
Finish installation `hh` using:
```bash
./configure && make && make install
```
In order to get autocompletion on Cygwin you must configure `hh` using:
```bash
hh --show-configuration >> ~/.bashrc
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).



CYGWIN
------
To install `hh` on [Cygwin](https://www.cygwin.com) clone Git repository:
```bash
git clone https://github.com/dvorka/hstr.git
```
Create build files using:
```bash
cd ./dist && ./1-dist.sh && cd ..
```
Finish installation `hh` using:
```bash
./configure && make && make install
```
In order to get autocompletion on Cygwin you must configure `hh` using:
```bash
hh --show-configuration >> ~/.bashrc
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).
