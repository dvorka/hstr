# HSTR
[![Travis CI](https://travis-ci.org/dvorka/hstr.svg?branch=master)](https://travis-ci.org/dvorka/hstr)
<!--
[![Current release](https://img.shields.io/github/release/dvorka/hstr.svg)](https://github.com/dvorka/hstr/releases)
[![GitHub issues](https://img.shields.io/github/issues/dvorka/hstr.svg?maxAge=360)](https://github.com/dvorka/hstr/issues)
[![All releases downloads](https://img.shields.io/github/downloads/dvorka/hstr/total.svg)](https://github.com/dvorka/hstr/releases)
-->

Easily view, navigate and search your **command history** with shell history suggest box for Bash and
[zsh](CONFIGURATION.md#zsh-history-settings).

[![Shell History Suggest Box](http://me.mindforger.com/projects/images/hh-animated-01.gif "Shell History Suggest Box @ YouTube")](http://www.youtube.com/watch?v=sPF29NyXe2U)


## Description
Are you looking for a command that you used **recently**? Do you
want to  avoid the need to write **long commands** over and over
again? Are you looking for a tool that is able to manage your 
**favorite** commands?

HSTR (**H**i**ST**o**R**y) is a command line utility that brings improved Bash command completion
from the history. It aims to make completion **easier** and more **efficient**
than <kbd>Ctrl</kbd><kbd>r</kbd>.

HSTR can also **manage** your command history (for instance you can remove
commands that are obsolete or contain a sensitive information) or **bookmark** 
your favorite commands.

Check [video](http://www.youtube.com/watch?v=sPF29NyXe2U) tutorial.


## Installation
Install:

* [Ubuntu](INSTALLATION.md#ubuntu)
* [Fedora](INSTALLATION.md#fedorarhelcentos)
* [Gentoo](INSTALLATION.md#gentoo)
* [CentOS](INSTALLATION.md#fedorarhelcentos)
* [openSUSE](INSTALLATION.md#opensuse)
* [RHEL](INSTALLATION.md#fedorarhelcentos)
* [Debian](INSTALLATION.md#debianmint)
* [Mint](INSTALLATION.md#debianmint)
* [Arch Linux](INSTALLATION.md#arch-linux)
* [Scientific Linux](INSTALLATION.md#fedorarhelcentos)
* [macOS](INSTALLATION.md#macos)

Build:

* [build on any Linux distro](INSTALLATION.md#build-on-any-linux-distro)
* [build snap](INSTALLATION.md#snap)
* [build on Ubuntu](INSTALLATION.md#build-on-ubuntu)
* [build on Debian](INSTALLATION.md#build-on-debian)
* [build on Fedora](INSTALLATION.md#build-on-fedora)
* [build on Windows Subsystem for Linux (WSL)](INSTALLATION.md#build-on-wsl)
* [build on Cygwin](INSTALLATION.md#build-on-cygwin)

Tarball:

* [build and install tarball](INSTALLATION.md#build-and-install-tarball)

## Configuration
Configure HSTR just by running:
```bash
hh --show-configuration >> ~/.bashrc
```
For detailed `hh` configuration documentation please refer to [Configuration](CONFIGURATION.md).


## Documentation
Check `hh` man page:

```bash
man hh
```

## In the News
Read about `hh` in [LinuxMagazine](http://www.linux-magazine.com/Issues/2014/164/Bash-History-on-Steroids), [UbuntuGeek](http://www.ubuntugeek.com/tag/hstr-bash-history), [DebianAdmin](http://www.debianadmin.com/hstr-easily-view-navigate-search-and-use-your-command-history-with-shell-history.html), [Tuxdiary](https://tuxdiary.com/2015/02/17/hstr/), [Softpedia](http://linux.softpedia.com/get/Terminals/BASH-Command-History-Completion-103155.shtml) or [OSTechNix](https://www.ostechnix.com/hstr-easily-view-navigate-search-manage-commandline-history/).


## Bugs
https://github.com/dvorka/hstr/issues
