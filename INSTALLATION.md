# Installation
Install:

* [Ubuntu](#ubuntu)
* [Fedora](#fedorarhelcentos)
* [Gentoo](#gentoo)
* [CentOS](#fedorarhelcentos)
* [openSUSE](#opensuse)
* [RHEL](#fedorarhelcentos)
* [Debian](#debianmint)
* [Mint](#debianmint)
* [Arch Linux](#arch-linux)
* [Scientific Linux](#fedorarhelcentos)
* [Windows Subsystem for Linux (WSL)](#wsl)
* [macOS](#macos)

Build:

* [build on any Linux distro](#build-on-any-linux-distro)
* [build on Ubuntu](#build-on-ubuntu)
* [build on Debian](#build-on-debian)
* [build on Fedora](#build-on-fedora)
* [build on WSL](#build-on-wsl)
* [build on Cygwin](#build-on-cygwin)

Tarball:

* [build and install tarball](#build-and-install-tarball)
# Install
Install HSTR using a package.

## Ubuntu
Install `hh` on Ubuntu using one-liner:

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

## Fedora/RHEL/Centos
Install `hh` on Fedora, RHEL or CentOS:

```bash
sudo dnf install hstr -y
```
... or:

```bash
sudo yum install hstr -y
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

---

If you want to make sure you have the latest version, then download `.rpm` archive from
[GitHub releases](https://github.com/dvorka/hstr/releases) and install it:

```bash
sudo dnf install ./hstr-<major>.<minor>.<revision>-2.x86_64.rpm -y
```

## Debian/Mint
Install `hh` from PPA. Add [my PPA](http://www.mindforger.com/debian), 
trust [GPG key](http://www.mindforger.com/gpgpubkey.txt) and install `hh`:

```bash
# add PPA to APT sources:
sudo echo -e "\ndeb http://www.mindforger.com/debian stretch main" >> /etc/apt/sources.list

# import PPA's GPG key
wget -qO - http://www.mindforger.com/gpgpubkey.txt | sudo apt-key add -

# update sources
sudo apt update

# install MindForger
sudo apt install hstr
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

---

Alternatively you can download and install `.deb` archive from [GitHub releases](https://github.com/dvorka/hstr/releases)
section of the project:

```bash
dpkg -i hstr_-<major>.<minor>.<revision>_1-amd64.deb
```


## openSUSE
To install `hh` on openSUSE Leap 42.1 run the following commands as root:

```bash
zypper addrepo http://download.opensuse.org/repositories/home:tuw-e184/openSUSE_Leap_42.1/home:tuw-e184.repo
zypper refresh
zypper install hstr
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## Gentoo
To install `hh` run (HSTR has been added to [Portage](https://bugs.gentoo.org/show_bug.cgi?id=527122)):

```bash
emerge app-shells/hstr
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## Arch Linux
Download latest distribution from [GitHub releases](https://github.com/dvorka/hstr/releases).

Use [PKGBUILD](https://wiki.archlinux.org/index.php/PKGBUILD) in the root of the distribution to build package using `makepkg`.

https://aur.archlinux.org/packages/hstr-git/

Install `hh`.

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## macOS
You can either install `hh` from [Homebrew](http://brew.sh/):

```bash
brew install hh
```

or from the source code as described further in this section. First install MacPorts:

   http://www.macports.org

And then install readline and ncurses:

```bash
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

## WSL
Install `hh` on [Windows Subsystem for Linux (WSL)](https://msdn.microsoft.com/en-us/commandline/wsl/about) using one-liner:

```bash
sudo add-apt-repository ppa:ultradvorka/ppa && sudo apt-get update && sudo apt-get install hh && hh --show-configuration >> ~/.bashrc
```

... or step by step:

```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt update
sudo apt install hh
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

# Build
Build HSTR from source code.

## Build on any Linux distro
Clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Create build files using:

```bash
cd ./build/tarball && ./tarball-automake.sh && cd ../..
```

Build and install `hh` using:

```bash
./configure && make && make install
```

Run HSTR:

```bash
hh
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## Build on Ubuntu
To build `hh` on Ubuntu clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Install dependencies:

```bash
sudo apt install automake gcc make libncursesw5-dev libreadline-dev
```

Create build files using:

```bash
cd ./build/tarball && ./tarball-automake.sh && cd ../..
```

Build and install `hh` using:

```bash
./configure && make && make install
```

Run HSTR:

```bash
hh
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## Build on Debian
To build `hh` on Ubuntu clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Install dependencies:

```bash
sudo apt install autotools gcc make libncursesw5-dev libreadline-dev
```

Create build files using:

```bash
cd ./build/tarball && ./tarball-automake.sh && cd ../..
```

Build and install `hh` using:

```bash
./configure && make && make install
```

Run HSTR:

```bash
hh
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## Build on Fedora
To build `hh` on Ubuntu clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Install dependencies:

```bash
sudo dnf install autotools ncurses-devel readline-devel
```

Create build files using:

```bash
cd ./build/tarball && ./tarball-automake.sh && cd ../..
```

Build and install `hh` using:

```bash
./configure && make && make install
```

Run HSTR:

```bash
hh
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## Build on WSL
To build `hh` on [Windows Subsystem for Linux (WSL)](https://msdn.microsoft.com/en-us/commandline/wsl/about) clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Install dependencies:

```bash
sudo apt install automake gcc make libncursesw5-dev libreadline-dev
```

Create build files using:

```bash
cd ./build/tarball && ./tarball-automake.sh && cd ../..
```

Build and install `hh` using:

```bash
./configure && make && make install
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

## Build on Cygwin
To build `hh` on [Cygwin](https://www.cygwin.com) clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Create build files using:

```bash
cd ./build/tarball && ./tarball-automake.sh && cd ../..
```

Build and install `hh` using:

```bash
./configure && make && make install
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).
# Tarball
Download and install HSTR tarball.

## Build and Install Tarball
Download latest tarball from [GitHub releases](https://github.com/dvorka/hstr/releases) section.
Expand the archive and install `hh` using:

```bash
./configure && make && make install
```

Optionally [configure](CONFIGURATION.md) `hh` and check its [man page](README.md#documentation).

