# Installation
Install:

* [Ubuntu](#ubuntu)
* [Fedora](#fedorarhelcentosscientific)
* [Gentoo](#gentoo)
* [CentOS](#fedorarhelcentosscientific)
* [openSUSE](#opensuse)
* [RHEL](#fedorarhelcentosscientific)
* [Debian](#debianmint)
* [Mint](#debianmint)
* [Arch Linux](#archmanjaro-linux)
* [Manjaro Linux](#archmanjaro-linux)
* [Scientific Linux](#fedorarhelcentos)
* [Haiku OS](#haiku-os)
* [macOS](#macos)

Build:

* [build on any Linux distro](#build-on-any-linux-distro)
* [build snap](#snap)
* [build on Ubuntu](#build-on-ubuntu)
* [build on Debian](#build-on-debian)
* [build on Fedora](#build-on-fedora)
* [build on Windows Subsystem for Linux (WSL)](#build-on-wsl)
* [build on Cygwin](#build-on-cygwin)
* [build on macOS](#build-on-macos)

Tarball:

* [build and install tarball](#build-and-install-tarball)
# Install
Install HSTR using a package.

## Ubuntu
Install HSTR on Ubuntu using one-liner:

```bash
sudo add-apt-repository ppa:ultradvorka/ppa && sudo apt-get update && sudo apt-get install hstr && hstr --show-configuration >> ~/.bashrc && . ~/.bashrc
```

... or step by step:

```bash
sudo add-apt-repository ppa:ultradvorka/ppa
sudo apt-get update
sudo apt-get install hstr
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Debian/Mint
Install HSTR from PPA. Add [my PPA](http://www.mindforger.com/debian), 
trust [GPG key](http://www.mindforger.com/gpgpubkey.txt) and install HSTR:

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

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

---

Alternatively you can download and install `.deb` archive from [GitHub releases](https://github.com/dvorka/hstr/releases)
section of the project:

```bash
dpkg -i hstr_-<major>.<minor>.<revision>_1-amd64.deb
```


## Fedora/RHEL/Centos/Scientific
Install HSTR on Fedora, RHEL, CentOS or Scientific Linux:

```bash
sudo dnf install hstr -y
```
... or:

```bash
sudo yum install hstr -y
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

---

If you want to make sure you have the latest version, then download `.rpm` archive from
[GitHub releases](https://github.com/dvorka/hstr/releases) and install it:

```bash
sudo dnf install ./hstr-<major>.<minor>.<revision>-2.x86_64.rpm -y
```

## openSUSE
To install HSTR on openSUSE Leap 42.1 run the following commands as root:

```bash
zypper addrepo http://download.opensuse.org/repositories/home:tuw-e184/openSUSE_Leap_42.1/home:tuw-e184.repo
zypper refresh
zypper install hstr
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Gentoo
To install HSTR on Gentoo run (HSTR has been added to [Portage](https://bugs.gentoo.org/show_bug.cgi?id=527122)):

```bash
emerge app-shells/hstr
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Arch/Manjaro Linux
To install HSTR on Arch or Manjaro Linux download latest distribution from [GitHub releases](https://github.com/dvorka/hstr/releases).

Use [PKGBUILD](https://wiki.archlinux.org/index.php/PKGBUILD) in the root of the distribution to build package using `makepkg`.

https://aur.archlinux.org/packages/hstr-git/

Install HSTR.

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Haiku OS
To install HSTR on Haiku OS use [Haiku Depot](https://www.haiku-os.org/docs/userguide/en/applications/haikudepot.html):

* https://depot.haiku-os.org/#!/?srchexpr=hstr

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).
## macOS
Install HSTR on macOS using [Homebrew](http://brew.sh/) ([formula](https://formulae.brew.sh/formula/hh)):

```bash
brew install hh
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

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

Build and install HSTR using:

```bash
./configure && make && make install
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Build snap
To build [snap](https://snapcraft.io/) for HSTR first clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Make sure that you can build HSTR from source code
as described in a "Build on ..." section for your
distribution.

Install [snapcraft](https://snapcraft.io/) and build
snap by running snapcraft in the root of Git repository:

```
$ ls ./snap
  snapcraft.yaml
$ snapcraft
  ...
```

Find `hstr_0+git..._amd64.snap` in the current directory.

## Build on Ubuntu
To build HSTR on Ubuntu clone Git repository:

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

Build and install HSTR using:

```bash
./configure && make && make install
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Build on Debian
To build HSTR on Debian clone Git repository:

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

Build and install HSTR using:

```bash
./configure && make && make install
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Build on Fedora
To build HSTR on Fedora clone Git repository:

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

Build and install HSTR using:

```bash
./configure && make && make install
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Build on WSL
To build HSTR on [Windows Subsystem for Linux (WSL)](https://msdn.microsoft.com/en-us/commandline/wsl/about) clone Git repository:

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

Build and install `hstr` using:

```bash
./configure && make && make install
```

It's **important** to finish installation by configuration of HSTR binding
which ensures propagation of chosen command to the prompt:

```bash
hstr --show-configuration >> ~/.bashrc
. ~/.bashrc
```

Make sure to **run** HSTR using <kbd>Ctrl</kbd><kbd>r</kbd> (or `hstrwsl`) - otherwise commands will not appear in prompt.

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Build on Cygwin
To build HSTR on [Cygwin](https://www.cygwin.com) clone Git repository:

```bash
git clone https://github.com/dvorka/hstr.git
```

Create build files using:

```bash
cd ./build/tarball && ./tarball-automake.sh && cd ../..
```

Build and install HSTR using:

```bash
./configure && make && make install
```

It's **important** to finish installation by configuration of HSTR binding
which ensures propagation of chosen command to the prompt:

```bash
hstr --show-configuration >> ~/.bashrc
. ~/.bashrc
```

Make sure to **run** HSTR using <kbd>Ctrl</kbd><kbd>r</kbd> (or `hstrcygwin`) - otherwise commands will not appear in prompt.

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).
## Build on macOS
HSTR can be built on macOS either using [MacPorts](https://www.macports.org/) or [Homebrew](https://brew.sh/).

---

To build HSTR using [MacPorts](https://www.macports.org/) install `readline` and `ncurses`:

```bash
port install readline
port install ncurses
```

Check `portfile` either on [macports.com](https://www.macports.org/ports.php?by=name&substr=hstr) or [GitHub](https://github.com/macports/macports-ports/blob/master/shells/hstr/Portfile):

```bash
autoreconf -fvi
./configure CFLAGS=-I/opt/local/include/ LDFLAGS=-L/opt/local/lib
make
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

---

To build HSTR using [Homebrew](https://brew.sh/):

```bash
autoreconf -fvi
./configure CFLAGS=-I$(brew --prefix)/opt/readline/include LDFLAGS=-L$(brew --prefix)/opt/readline/lib
make
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

# Tarball
Download and install HSTR tarball.

## Build and Install Tarball
Download latest tarball from [GitHub releases](https://github.com/dvorka/hstr/releases) section.
Expand the archive and install HSTR using:

```bash
./configure && make && make install
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).
