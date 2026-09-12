# HSTR

[![Support on Open Collective](https://img.shields.io/badge/Sponsor-Open%20Collective-blue?style=for-the-badge&logo=open-collective)](https://opencollective.com/dvorka-floss-lab)
[![GitHub Actions CI for Ubuntu](https://github.com/dvorka-oss/hstr/actions/workflows/build_ubuntu.yml/badge.svg)](https://github.com/dvorka-oss/hstr/actions)
[![Current release](https://img.shields.io/github/release/dvorka/hstr.svg)](https://github.com/dvorka-oss/hstr/releases)
[![GitHub issues](https://img.shields.io/github/issues/dvorka/hstr.svg?maxAge=360)](https://github.com/dvorka-oss/hstr/issues)
[![All releases downloads](https://img.shields.io/github/downloads/dvorka/hstr/total.svg)](https://github.com/dvorka-oss/hstr/releases)
<!--
[![License](https://img.shields.io/github/license/dvorka/hstr?color=%23fe0000)](https://github.com/dvorka-oss/hstr/blob/master/LICENSE)
-->

Easily view, navigate and search your **command history** with shell history TUI suggest box for
[bash](https://www.gnu.org/software/bash/) and [zsh](CONFIGURATION.md#zsh-history-settings).

[![Shell History Suggest Box](doc/hstr-v2.gif "Shell History Suggest Box @ YouTube")](http://www.youtube.com/watch?v=sPF29NyXe2U)

## Description

Are you looking for a command that you used **recently**? Do you
want to  avoid the need to write **long commands** over and over
again? Are you looking for a tool that is able to manage your
**favorite** commands?

HSTR (**H**i**ST**o**R**y) is a command line utility that brings improved `bash`/`zsh` command completion
from the history. It aims to make completion **easier** and more **efficient** than <kbd>Ctrl-r</kbd>. Press <kbd>TAB</kbd> to edit a selected command and <kbd>ENTER</kbd> to execute it directly.

HSTR can also **manage** your command history (for instance you can remove
commands that are obsolete or contain a piece of sensitive information)
or **bookmark** your favorite commands.

<!-- Check [video](http://www.youtube.com/watch?v=sPF29NyXe2U) tutorial. -->

## Packages

<table><tr><td>
  <a href="https://repology.org/project/hstr/versions">
    <img src="https://repology.org/badge/vertical-allrepos/hstr.svg?columns=4" alt="Packaging status" align="left">
  </a>
</td></tr></table>

## Installation

Install:

* [Ubuntu](INSTALLATION.md#ubuntu)
* [Debian](INSTALLATION.md#debianmint)
* [Mint](INSTALLATION.md#debianmint)
* [Fedora](INSTALLATION.md#fedorarhelcentosscientific)
* [CentOS](INSTALLATION.md#fedorarhelcentosscientific)
* [RHEL](INSTALLATION.md#fedorarhelcentosscientific)
* [Scientific Linux](INSTALLATION.md#fedorarhelcentosscientific)
* [openSUSE](INSTALLATION.md#opensuse)
* [Gentoo](INSTALLATION.md#gentoo)
* [Alpine Linux](INSTALLATION.md#alpine-linux)
* [Arch](INSTALLATION.md#archmanjaro-linux)
* [Manjaro Linux](INSTALLATION.md#archmanjaro-linux)
* [FreeBSD](INSTALLATION.md#freebsd)
* [Slackware Linux](INSTALLATION.md#slackware)
* [Void Linux](INSTALLATION.md#void-linux)
* [Milis Linux](INSTALLATION.md#milis-linux)
* [Haiku OS](INSTALLATION.md#haiku-os)
* [macOS](INSTALLATION.md#macos)
* [nix package manager](INSTALLATION.md#nix-package-manager)
* [Guix package manager](INSTALLATION.md#guix-package-manager)
* [MPR package manager](INSTALLATION.md#mpr-package-manager)

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

Check also [packages](https://pkgs.org/search/?q=hstr) for Linux and Unix.

## Configuration

Configure HSTR just by running:

```bash
# bash
hstr --show-bash-configuration >> ~/.bashrc

# zsh
hstr --show-zsh-configuration >> ~/.zshrc
```

For detailed HSTR configuration documentation please refer to [Configuration](CONFIGURATION.md).

## Tips
Troubleshooting:

* [Commands chosen in HSTR are not inserted to terminal](INSTALLATION.md#hstr-2x-to-3x-ioctl--tiocsti)

Tips and tricks:

* [How to hide commands from your history](./TIPS.md)
* [How to use command tags for fast lookup](./TIPS.md)
* [How to curate your favorite commands](./TIPS.md)

## Perfect Software
[Perfect software](https://outofdesk.netlify.app/perfect-software) is not necessarily the best software - it is perfect simply because it does exactly what you want, how do you want it and when you want it. The [YAGNI](https://martinfowler.com/bliki/Yagni.html) - You Aren't Gonna Need It - mantra protects software from wasting time and resources on implementing anything and everything.

The **HSTR** project **is perfect to me**. Even if project's main branch does not receive updates frequently, be sure that I use it every day - many many times. I add a small feature here or there or fix a bug. In other words, the project is **not** and will **not** be dead.

If you have any ideas, suggestions, news to share, or constructive criticism to share, **do not hesitate to contact me**! Please do not be offended if your GitHub issue or bug report is not implemeted - IAGNI.

## In the News

Read about HSTR in:

* [LinuxMagazine](http://www.linux-magazine.com/Issues/2014/164/Bash-History-on-Steroids)
* [UbuntuGeek](http://www.ubuntugeek.com/tag/hstr-bash-history)
* [DebianAdmin](http://www.debianadmin.com/hstr-easily-view-navigate-search-and-use-your-command-history-with-shell-history.html)
* [Tuxdiary](http://tuxdiary.com/2015/02/17/hstr/)
* [Softpedia](http://linux.softpedia.com/get/Terminals/BASH-Command-History-Completion-103155.shtml)
* [OSTechNix](https://www.ostechnix.com/hstr-easily-view-navigate-search-manage-commandline-history/)


## Documentation

Check man page:

```bash
man hstr
```

User videos: [Zack](https://www.youtube.com/watch?v=Qd75pIeQkH8), [Dvorka](https://www.youtube.com/watch?v=sPF29NyXe2U), [Yayuuu](https://www.reddit.com/r/commandline/comments/1lo5up5/hstr_tiny_bash_script_that_helps_you_browse_and/), and [Yu-Jie Lin](https://www.youtube.com/watch?v=Qx5n_5B5xUw).

## Bugs

https://github.com/dvorka-oss/hstr/issues
