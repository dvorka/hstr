# Configuration
Get most of HSTR by configuring it with:

```bash
# bash
hstr --show-configuration >> ~/.bashrc

# zsh
hstr --show-configuration >> ~/.zshrc
```

Run `hstr --show-configuration` to determine what will be appended to your
bash / zsh profile. Don't forget to `source ~/.bashrc` / `source ~/.zshrc`
to apply changes.

---

For more configuration options details please refer to:

* bind HSTR to a [keyboard shortcut](#bash-binding-hstr-to-keyboard-shortcut)
    * [Bash Emacs keymap](#bash-emacs-keymap--default) (default)
    * [Bash Vim keymap](#bash-vim-keymap)
    * [zsh Emacs keymap](#zsh-emacs-keymap) (default)
* create `hh` [alias](Alias) for `hstr`
* use [environment variables](#environment-variables) and [HSTR config options](#hstr-config-options)
    * get more [colors](#colors)
    * change [positioning of elements](#positioning)
    * choose [default history view](#default-history-view)
    * set [filtering preferences](#filtering)
    * set up [static favorites](#static-favorites) or [skip favorites comments](#skip-favorites-comments)
    * configure commands [blacklist](#blacklist)
    * disable [confirm on delete](#confirm-on-delete)
    * tune [verbosity](#verbosity)
    * some [examples](#examples)
* history settings:
    * [Bash history settings](#bash-history-settings)
    * [zsh history settings](#zsh-history-settings)

Don't miss the HSTR config [examples](#examples).

## Bash Binding HSTR to Keyboard Shortcut
`Bash` uses *Emacs* style keyboard shortcuts by default. There is
also *Vi* mode. Find out how to bind HSTR to a keyboard shortcut 
based on the style you prefer below.

Check your active bash keymap with:

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


### Bash Emacs Keymap (default)
Bind HSTR to a `bash` key e.g. to <kbd>Ctrl-r</kbd>:

```bash
bind '"\C-r": "\C-a hstr -- \C-j"'
```

or <kbd>Ctrl-Alt-r</kbd>:

```bash
bind '"\e\C-r":"\C-a hstr -- \C-j"'
```

or <kbd>Ctrl-F12</kbd>:

```bash
bind '"\e[24;5~":"\C-a hstr -- \C-j"'
```

Bind HSTR to <kbd>Ctrl-r</kbd> only if it is interactive shell:

```bash
if [[ $- =~ .*i.* ]]; then bind '"\C-r": "\C-a hstr -- \C-j"'; fi
```

You can bind also other HSTR commands like `--kill-last-command`:

```bash
if [[ $- =~ .*i.* ]]; then bind '"\C-xk": "\C-a hstr -k \C-j"'; fi
```

### Bash Vim Keymap
Bind HSTR to a `bash` key e.g. to <kbd>Ctrl-r</kbd>:

```bash
bind '"\C-r": "\e^ihstr -- \n"'
```

### Zsh Emacs Keymap
Bind HSTR to a `zsh` key e.g. to <kbd>Ctrl-r</kbd>:

```bash
bindkey -s "\C-r" "\C-a hstr -- \C-j"
```


## Alias
If you want to make running of `hstr` from command line even easier, 
then define alias in your `~/.bashrc`:

```bash
alias hh=hstr
```

Don't forget to source `~/.bashrc` to be able to to use `hh` command.


## Environment variables
- `HISTFILE` (defaults to `~/.bash_history` or `~/.zsh_history`)
- `HSTR_PROMPT` (defaults to `<user>@<hostname>$ `)
- `HSTR_IS_SUBSHELL` (when HSTR is used in a subshell, set to `1` to fix output when pressing `TAB` or `RIGHT` arrow key)
- `HSTR_CONFIG` (see below)


## HSTR Config Options
HSTR reads the environment variable `HSTR_CONFIG` for a **comma-separated list** of options.


### Colors
Let HSTR to use colors:

```bash
export HSTR_CONFIG=hicolor
```

or ensure black and white mode:

```bash
export HSTR_CONFIG=monochromatic
```

### Positioning
To show the prompt at the bottom of the screen (instead at the top) use:

```bash
export HSTR_CONFIG=prompt-bottom
```

To show the basic help and history help labels on the opposite site (instead of next to the prompt) use:

```bash
export HSTR_CONFIG=help-on-opposite-side
```

To hide the basic help label:

```bash
export HSTR_CONFIG=hide-basic-help
```

To hide both the basic help and history help labels:

```bash
export HSTR_CONFIG=hide-help
```

### Default History View
To show normal history by default (instead of metrics-based view, which is default)
use:

```bash
export HSTR_CONFIG=raw-history-view
```

To show favorite commands as default view use:

```bash
export HSTR_CONFIG=favorites-view
```

### Filtering
To use regular expressions based matching:

```bash
export HSTR_CONFIG=regexp-matching
```

To use substring based matching:

```bash
export HSTR_CONFIG=substring-matching
```

To use keywords (substrings whose order doesn't matter) search matching (default):

```bash
export HSTR_CONFIG=keywords-matching
```

---

Make search case sensitive (insensitive by default):

```bash
export HSTR_CONFIG=case-sensitive
```

Keep duplicates in `raw-history-view` (duplicate commands are discarded by default):

```bash
export HSTR_CONFIG=duplicates
```

### Static favorites
Last selected favorite command is put the head of favorite commands list
by default. If you want to disable this behavior and make favorite
commands list static, then use the following configuration:

```bash
export HSTR_CONFIG=static-favorites
```

### Skip favorites comments
If you don't want to show lines starting with `#` (comments) among
favorites, then use the following configuration:

```bash
export HSTR_CONFIG=skip-favorites-comments
```

### Blacklist
Skip commands when processing ranking view history. Use of blacklist
file is **disabled** by default - you can enable it by adding `blacklist`
to `HSTR_CONFIG` environment variable:

```bash
export HSTR_CONFIG=blacklist
```

Commands to be stored in `~/.hstr_blacklist` file with trailing empty line. For instance:

```
cd
my-private-command
ls
ll
```

### Confirm on Delete
Do not prompt for confirmation when deleting history items:

```bash
export HSTR_CONFIG=no-confirm
```

### Verbosity
Show a message when deleting the last command from history:

```bash
export HSTR_CONFIG=verbose-kill
```

Show warnings:

```bash
export HSTR_CONFIG=warning
```

Show debug messages:

```bash
export HSTR_CONFIG=debug
```

### Examples
More colors with case sensitive search of history:

```bash
export HSTR_CONFIG=hicolor,case-sensitive
```

Favorite commands view in black and white with prompt at the bottom of the screen:

```bash
export HSTR_CONFIG=favorites-view,prompt-bottom
```

Keywords based search in colors with debug mode verbosity:

```bash
export HSTR_CONFIG=keywords-matching,hicolor,debug
```

## Bash History Settings
Use the following `bash` settings to get most out of HSTR.

Increase the size of history maintained by `bash` - variables defined below 
increase the number of history items and history file size (default value is 500):

```bash
export HISTFILESIZE=10000
export HISTSIZE=${HISTFILESIZE}
```

Ensure syncing (flushing and reloading) of `.bash_history` with 
in-memory history:

```bash
export PROMPT_COMMAND="history -a; history -n; ${PROMPT_COMMAND}"
```

Force appending of in-memory history to `.bash_history` (instead 
of overwriting):

```bash
shopt -s histappend
```

Use leading space to hide commands from history:

```bash
export HISTCONTROL=ignorespace
```

Suitable for a sensitive information like passwords.
## Zsh History Settings
If you use `zsh`, set `HISTFILE` environment variable in `~/.zshrc`:

```
export HISTFILE=~/.zsh_history
```

Please check your system configuration - history might be store in `~/.zsh_history`, `~/.zhistory` or other file.
