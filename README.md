# HSTR
[![Travis CI](https://travis-ci.org/dvorka/hstr.svg?branch=master)](https://travis-ci.org/dvorka/hstr)
[![Current release](https://img.shields.io/github/release/dvorka/hstr.svg)](https://github.com/dvorka/hstr/releases)
[![GitHub issues](https://img.shields.io/github/issues/dvorka/hstr.svg?maxAge=360)](https://github.com/dvorka/hstr/issues)
[![All releases downloads](https://img.shields.io/github/downloads/dvorka/hstr/total.svg)](https://github.com/dvorka/hstr/releases)
[![License](https://img.shields.io/github/license/dvorka/hstr?color=%23fe0000)](https://github.com/dvorka/hstr/blob/master/LICENSE)

Easily view, navigate and search your **command history** with shell history suggest box for 
[bash](https://www.gnu.org/software/bash/) and [zsh](CONFIGURATION.md#zsh-history-settings).

[![Shell History Suggest Box](doc/hstr-v2.gif "Shell History Suggest Box @ YouTube")](http://www.youtube.com/watch?v=sPF29NyXe2U)

## Description
Are you looking for a command that you used **recently**? Do you
want to  avoid the need to write **long commands** over and over
again? Are you looking for a tool that is able to manage your
**favorite** commands?

HSTR (**H**i**ST**o**R**y) is a command line utility that brings improved `bash`/`zsh` command completion
from the history. It aims to make completion **easier** and more **efficient**
than <kbd>Ctrl-r</kbd>.

HSTR can also **manage** your command history (for instance you can remove
commands that are obsolete or contain a piece of sensitive information)
or **bookmark** your favorite commands.

<!-- Check [video](http://www.youtube.com/watch?v=sPF29NyXe2U) tutorial. -->


## Installation
Install:

* [Ubuntu](INSTALLATION.md#ubuntu)
* [Fedora](INSTALLATION.md#fedorarhelcentosscientific)
* [Gentoo](INSTALLATION.md#gentoo)
* [CentOS](INSTALLATION.md#fedorarhelcentosscientific)
* [openSUSE](INSTALLATION.md#opensuse)
* [RHEL](INSTALLATION.md#fedorarhelcentosscientific)
* [Debian](INSTALLATION.md#debianmint)
* [Mint](INSTALLATION.md#debianmint)
* [Arch Linux](INSTALLATION.md#archmanjaro-linux)
* [Manjaro Linux](INSTALLATION.md#archmanjaro-linux)
* [Scientific Linux](INSTALLATION.md#fedorarhelcentosscientific)
* [Haiku OS](INSTALLATION.md#haiku-os)
* [macOS](INSTALLATION.md#macos)
* [nix package manager](INSTALLATION.md#nix-package-manager)
* [Guix package manager](INSTALLATION.md#guix-package-manager)

Build:

* [build on any Linux distro](INSTALLATION.md#build-on-any-linux-distro)
* [build snap](INSTALLATION.md#snap)
* [build on Ubuntu](INSTALLATION.md#build-on-ubuntu)
* [build on Debian](INSTALLATION.md#build-on-debian)
* [build on Fedora](INSTALLATION.md#build-on-fedora)
* [build on Windows Subsystem for Linux (WSL)](INSTALLATION.md#build-on-wsl)
* [build on Cygwin](INSTALLATION.md#build-on-cygwin)
* [build on macOS](INSTALLATION.md#build-on-macos)

Tarball:

* [build and install tarball](INSTALLATION.md#build-and-install-tarball)


## Configuration
Configure HSTR just by running:

```bash
# bash
hstr --show-configuration >> ~/.bashrc

# zsh
hstr --show-configuration >> ~/.zshrc
```
For detailed HSTR configuration documentation please refer to [Configuration](CONFIGURATION.md).


## Documentation
Check man page:

```bash
man hstr
```

User videos: [Zack](https://www.youtube.com/watch?v=Qd75pIeQkH8), [Dvorka](https://www.youtube.com/watch?v=sPF29NyXe2U) and [Yu-Jie Lin](https://www.youtube.com/watch?v=Qx5n_5B5xUw).
## In the News
Read about HSTR in [LinuxMagazine](http://www.linux-magazine.com/Issues/2014/164/Bash-History-on-Steroids), [UbuntuGeek](http://www.ubuntugeek.com/tag/hstr-bash-history), [DebianAdmin](http://www.debianadmin.com/hstr-easily-view-navigate-search-and-use-your-command-history-with-shell-history.html), [Tuxdiary](http://tuxdiary.com/2015/02/17/hstr/), [Softpedia](http://linux.softpedia.com/get/Terminals/BASH-Command-History-Completion-103155.shtml) and [OSTechNix](https://www.ostechnix.com/hstr-easily-view-navigate-search-manage-commandline-history/).


## Upgrade
If you are HSTR 1.x user, please read about [HSTR 2.x changes](https://github.com/dvorka/hstr/releases/tag/2.0).


## Bugs
https://github.com/dvorka/hstr/issues
