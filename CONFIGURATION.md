CONFIGURATION
=============
Get most of HSTR by configuring it with:
```bash
hh --show-configuration >> ~/.bashrc
```
Run `hh --show-configuration` to determine what will be appended to your Bash profile.

For more configuration options details please refer to:
* bind `hh` command to a [keyboard shortcut](#binding-hh-to-keyboard-shortcut)
* get more [colors](#colors)
* choose [default history view](#history-view)
* [command blacklist](#blacklist)
* [verbosity](#verbosity)
* [Bash history settings](#bash-history-settings)
* [zsh history settings](#zsh-history-settings)

Check also configuration [examples](#examples).


BINDING HSTR TO KEYBOARD SHORTCUT
---------------------------------
Bash uses *Emacs* style keyboard shortcuts by default. There is
also *Vi* mode. Find out how to bind HSTR to a keyboard shortcut
based on the style you prefer below.

Check your active Bash keymap with:
```bash
bind -v | grep editing-mode
bind -v | grep keymap
```

To determine character sequence emitted by a pressed key in
terminal, type <kbd>Ctrl-v</kbd> and then press the key. Check your
current bindings using:
```bash
bind -S
```


## BASH EMACS KEYMAP (DEFAULT)
Bind `hh` to a Bash key e.g. to <kbd>Ctrl-r</kbd>:
```bash
bind '"\C-r": "\C-ahh -- \C-j"'
```
or <kbd>Ctrl-Alt-r</kbd>:
```bash
bind '"\e\C-r":"\C-ahh -- \C-j"'
```
or <kbd>Ctrl-F12</kbd>:
```bash
bind '"\e[24;5~":"\C-ahh -- \C-j"'
```
Bind `hh` to <kbd>Ctrl-r</kbd> only if it is interactive shell:
```bash
if [[ $- =~ .*i.* ]]; then bind '"\C-r": "\C-a hh -- \C-j"'; fi
```

## BASH VI KEYMAP
Bind `hh` to a Bash key e.g. to <kbd>Ctrl-r</kbd>:
```bash
bind '"\C-r": "\C-ahh -- \C-j"'
```

## ZSH EMACS KEYMAP (DEFAULT)
Bind `hh` to a Zsh key e.g. to <kbd>Ctrl-r</kbd>:
```bash
bindkey -s "\C-r" "\eqhh --\n"
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
Flip the view so the prompt is at the bottom:
```bash
export HH_CONFIG=prompt-bottom
```
Choose a minimal layout (currently this just removes the help bar):
```bash
export HH_CONFIG=minimal
```


FILTERING
---------
Make search case sensitive (insensitive by default):
```bash
export HH_CONFIG=casesensitive
```
Keep duplicates in `rawhistory` view (duplicates discarded by default):
```bash
export HH_CONFIG=duplicates
```


BLACKLIST
---------
Skip commands when processing history i.e. make sure that these commands
will *not* be shown in any view:
```bash
export HH_CONFIG=blacklist
```
Commands to be stored in `~/.hh_blacklist` file with trailing empty line. For instance:
```
cd
my-private-command
ls
ll

```

VERBOSITY
---------
Show warnings:
```bash
export HH_CONFIG=warning
```
Show debug messages:
```bash
export HH_CONFIG=debug
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

ZSH HISTORY SETTINGS
--------------------
If you use `zsh`, set HISTFILE environment variable in ~/.zshrc:

```
export HISTFILE=~/.zsh_history
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
