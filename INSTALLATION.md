INSTALLATION
============
Install `hh` using [distribution](#distribution-installation), from [source code](#installation-from-source-code),
on [Ubuntu](#ubuntu), [Debian/Mint](#debianmint), [Arch Linux](#arch-linux)
or [Mac OS](#mac-os).


UBUNTU
------
Install `hh` on Ubuntu:
```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt-get update
sudo apt-get install hh
```
Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

FEDORA & RHEL
-------------

With ```dnf``` available:

```bash
sudo dnf copr enable tbabej/hstr -y
sudo dnf install hstr -y
```

For Fedora based systems without dnf available:

1. Download the repo directly from the
[copr page](https://copr.fedoraproject.org/coprs/tbabej/hstr/)
and save it into ```/etc/yum.repos.d/``` directory.
1. ```sudo yum install hstr -y```


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


ARCH LINUX
----------
Download latest distribution from project Releases section:

https://github.com/dvorka/hstr/releases

Use [PKGBUILD](https://wiki.archlinux.org/index.php/PKGBUILD) in the root of the distribution to build package using `makepkg`:

https://aur.archlinux.org/packages/hstr-git/

Install `hh`.

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).


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

Build and install:
```bash
autoreconf -fvi
./configure CFLAGS=-I$(brew --prefix)/opt/readline/include LDFLAGS=-L$(brew --prefix)/opt/readline/lib
make
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).
