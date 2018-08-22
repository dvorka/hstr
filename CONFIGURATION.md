# Configuration
Get most of HSTR by configuring it with:

```bash
hh --show-configuration >> ~/.bashrc
```

Run `hh --show-configuration` to determine what will be appended to your Bash profile. Don't forget
to source `~/.bashrc` to apply changes.

---

For more configuration options details please refer to:

* Bind `hh` to a [keyboard shortcut](#binding-hh-to-keyboard-shortcut)
* Get more [colors](#colors)
* Choose [default history view](#history-view)
* Set [filtering preferences](#filtering)
* Configure commands [blacklist](#blacklist)
* Disable [confirm on delete](#confirm-on-delete)
* Tune [verbosity](#verbosity)
* Bash:
    * [Bash Emacs keymap](#bash-emacs-keymap-default) (default)
    * [Bash Vim keymap](#bash-vim-keymap-default)
    * [Bash history settings](#bash-history-settings)
* zsh:
    * [zsh Emacs keymap](#zsh-emacs-keymap-default) (default)
    * [zsh history settings](#zsh-history-settings)

Check also configuration [examples](#examples).

## Alias
Make running of `hstr` (e.g. if you don't want to bind it to <kbd>Ctrl+r</kbd>)
even easier by defining alias in your `~/.bashrc`:

```bash
alias hh=hstr
```
## Binding HSTR to Keyboard Shortcut
Bash uses *Emacs* style keyboard shortcuts by default. There is
also *Vi* mode. Find out how to bind HSTR to a keyboard shortcut 
based on the style you prefer below.

Check your active Bash keymap with:

```bash
bind -v | grep editing-mode
bind -v | grep keymap
``` 

To determine character sequence emitted by a pressed key in 
terminal, type <kbd>Ctrl</kbd><kbd>v</kbd> and then press the key. Check your 
current bindings using:

```bash
bind -S
```


## Colors
Get `hh` in more colors:

```bash
export HH_CONFIG=hicolor
```

or ensure black and white mode:

```bash
export HH_CONFIG=monochromatic
```

## History View
Show normal history by default (instead of metrics-based view):

```bash
export HH_CONFIG=rawhistory
```

Show favorite commands by default (instead of metrics-based view):

```bash
export HH_CONFIG=favorites
```

## Filtering
Make search case sensitive (insensitive by default):

```bash
export HH_CONFIG=casesensitive
```

Keep duplicates in `rawhistory` view (duplicates discarded by default):

```bash
export HH_CONFIG=duplicates
```

## Static favorites
Latest favorite command is put the head of favorite commands list
by default. If you want to disable this behavior and make favorite
commands list static, then use the following configuration:

```bash
export HH_CONFIG=static-favorites
```
## Blacklist
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

## Confirm on Delete
Do not prompt for confirmation when deleting history items:

```bash
export HH_CONFIG=noconfirm
```

## Verbosity
Show a message when deleting the last command from history:

```bash
export HH_CONFIG=verbose-kill
```

Show warnings:

```bash
export HH_CONFIG=warning
```

Show debug messages:

```bash
export HH_CONFIG=debug
```

## Bash Emacs Keymap (default)
Bind `hh` to a Bash key e.g. to <kbd>Ctrl</kbd><kbd>r</kbd>:

```bash
bind '"\C-r": "\C-ahh -- \C-j"'
```

or <kbd>Ctrl</kbd><kbd>Alt</kbd><kbd>r</kbd>:

```bash
bind '"\e\C-r":"\C-ahh -- \C-j"'
```

or <kbd>Ctrl</kbd><kbd>F12</kbd>:

```bash
bind '"\e[24;5~":"\C-ahh -- \C-j"'
```

Bind `hh` to <kbd>Ctrl</kbd><kbd>r</kbd> only if it is interactive shell:

```bash
if [[ $- =~ .*i.* ]]; then bind '"\C-r": "\C-a hh -- \C-j"'; fi
```

You can bind also other `hh` commands like `-kill-last-command`:

```bash
if [[ $- =~ .*i.* ]]; then bind '"\C-xk": "\C-a hh -k \C-j"'; fi
```

## Bash Vim Keymap
Bind `hh` to a Bash key e.g. to <kbd>Ctrl</kbd><kbd>r</kbd>:

```bash
bind '"\C-r": "\e0ihh -- \C-j"'
```


## Bash History Settings
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

## Zsh Emacs Keymap (default)
Bind `hh` to a `zsh` key e.g. to <kbd>Ctrl</kbd><kbd>r</kbd>:

```bash
bindkey -s "\C-r" "\eqhh --\n"
```


## Zsh History Settings
If you use `zsh`, set `HISTFILE` environment variable in `~/.zshrc`:

```
export HISTFILE=~/.zsh_history
```


## Examples
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
