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
* [Milis Linux](#milis-linux)
* [Manjaro Linux](#archmanjaro-linux)
* [Scientific Linux](#fedorarhelcentosscientific)
* [Haiku OS](#haiku-os)
* [macOS](#macos)
* [nix package manager](#nix-package-manager)
* [Guix package manager](#guix-package-manager)

Build:

* [build on any Linux distro](#build-on-any-linux-distro)
* [build snap](#build-snap)
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
Install HSTR from PPA. Add [my PPA](https://www.mindforger.com/debian),
trust [GPG key](https://www.mindforger.com/gpgpubkey.txt) and install HSTR:

```bash
# add PPA to APT sources:
sudo echo -e "\ndeb https://www.mindforger.com/debian stretch main" >> /etc/apt/sources.list

# import PPA's GPG key
wget -qO - https://www.mindforger.com/gpgpubkey.txt | sudo apt-key add -

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
wget -O hstr.deb https://github.com/dvorka/hstr/releases/download/2.2/hstr_<major>.<minor>.<revision>-1_amd64.deb

# dependencies:
apt-get install libncursesw5 libtinfo5

sudo dpkg -i hstr.deb
```


## Fedora/RHEL/CentOS/Scientific
Install HSTR on Fedora, RHEL, CentOS or Scientific Linux:

```bash
sudo dnf install hstr
```
... or:

```bash
sudo yum install hstr
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

---

If you want to make sure you have the latest version, then download `.rpm` archive from
[GitHub releases](https://github.com/dvorka/hstr/releases) and install it:

```bash
sudo dnf install ./hstr-<major>.<minor>.<revision>-2.x86_64.rpm
```

## openSUSE
To install HSTR on openSUSE, please check https://software.opensuse.org/package/hstr first. You can install HSTR as shown below (just choose repository you like):

```bash
zypper addrepo https://download.opensuse.org/repositories/home:lemmy04/openSUSE_Leap_15.1/home:lemmy04.repo
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
To install HSTR on Arch or Manjaro Linux download the latest distribution from [GitHub releases](https://github.com/dvorka/hstr/releases).

Use [PKGBUILD](https://wiki.archlinux.org/index.php/PKGBUILD) in the root of the distribution to build package using `makepkg`.

To install HSTR from the [Arch User Repository (AUR)](https://aur.archlinux.org/packages/hstr-git/) run `yaourt -S hstr-git` in command line.

## Milis Linux
To install HSTR on Milis Linux run the following commands as root:

```bash
mps kur hstr
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Haiku OS
To install HSTR on Haiku OS use [Haiku Depot](https://www.haiku-os.org/docs/userguide/en/applications/haikudepot.html):

* https://depot.haiku-os.org/#!/?srchexpr=hstr

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).
## macOS
Install HSTR on macOS using [Homebrew](https://brew.sh/) ([formula](https://formulae.brew.sh/formula/hstr)):

```bash
brew install hstr
```

[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## nix package manager
To install HSTR using the [nix package manager](https://nixos.org/nix/) e.g. on [NixOS](https://nixos.org/) you can use the [nix derivation for HSTR](https://github.com/NixOS/nixpkgs/blob/master/pkgs/applications/misc/hstr/default.nix):

```bash
nix-env -i hstr
```


[Configure](CONFIGURATION.md) HSTR and check its [man page](README.md#documentation).

## Guix package manager
To install HSTR in [GuixSD](https://www.gnu.org/software/guix/) or using the standalone [Guix package manager](https://www.gnu.org/software/guix/manual/en/html_node/Binary-Installation.html#Binary-Installation) you can install the `hstr` package, e.g.

```bash
guix package -i hstr
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

Make sure to **run** HSTR using <kbd>Ctrl-r</kbd> (or `hstrwsl`) - otherwise commands will not appear in prompt.

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

Make sure to **run** HSTR using <kbd>Ctrl-r</kbd> (or `hstrcygwin`) - otherwise commands will not appear in prompt.

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
