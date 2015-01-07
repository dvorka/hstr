CONFIGURATION
=============
Get most of `hh` by configuring it with:
```bash
hh --show-configuration >> ~/.bashrc
```
Run `hh --show-configuration` to determine what will be appended to your BASH profile.

This document describes `hh` related configuration in detail - 
[binding hh to a keyboard shortcut](#binding-hh-to-keyboard-shortcut),
[colors](#colors),
[default history view](#history-view),
[verbosity](#verbosity),
[BASH history settings](#bash-history-settings)
and [examples](#examples).



BINDING HH TO KEYBOARD SHORTCUT
-------------------------------
Bind `hh` to a BASH key e.g. to `Ctrl-r`:
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
Bind `hh` to `Ctrl-r` only if it is interactive shell:
```bash
if [[ $- =~ .*i.* ]]; then bind '"\C-r": "\C-a hh \C-j"'; fi
```

To determine the character sequence emitted by a pressed key in terminal, 
type `Ctrl-v` and then press the key. Check your current bindings using:
```bash
bind -S
```

COLORS
------
Get `hh` in more colors:
```bash
export HH_CONFIG=hicolor
```
or ensure black and white mode:
```bash
export HH_CONFIG=monochromatic
```

HISTORY VIEW
------------
Show normal history by default (instead of metrics-based view):
```bash
export HH_CONFIG=rawhistory
```
Show favorite commands by default (instead of metrics-based view):
```bash
export HH_CONFIG=favorites
```

FILTERING
---------
Make search case sensitive (insensitive by default):
```bash
export HH_CONFIG=casesensitive
```

VERBOSITY
-------
Show warnings:
```bash
export HH_CONFIG=warning
```
Show debug messages:
```bash
export HH_CONFIG=debug
```

EXAMPLES
--------
More colors with case sensitive search of history:
```bash
export HH_CONFIG=hicolor,casesensitive
```
Favorite commands view in black and white:
```bash
export HH_CONFIG=favorites,monochromatic
```
Keywords based search in colors with debug mode verbosity:
```bash
export HH_CONFIG=keywords,hicolor,debug
```


BASH HISTORY SETTINGS
---------------------
Use the following BASH settings to get most out of `hh`.

Increase the size of history maintained by BASH - variables defined below increase the 
number of history items and history file size (default value is 500):
```bash
export HISTFILESIZE=10000
export HISTSIZE=${HISTFILESIZE}
```
Ensure syncing (flushing and reloading) of `.bash_history` with in-memory 
  history:
```bash
export PROMPT_COMMAND="history -a; history -n; ${PROMPT_COMMAND}"
```

Force appending of in-memory history to `.bash_history` 
  (instead of overwriting): 
```bash
shopt -s histappend
```

Use leading space to hide commands from history:
```bash
export HISTCONTROL=ignorespace
```
Suitable for a sensitive information like passwords.
