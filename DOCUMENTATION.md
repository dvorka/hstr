This document is an unfinished draft.

# Documentation
Table of contents:

* Installation
  * Distribution
  * Source code
  * Ubuntu
  * Debian
  * Mint
  * Arch
  * Mac OS
* Configuration
  * Keyboard shortcut
  * Colors
  * View
  * Verbosity
  * Shell specific
    * Bash
    * Zsh
* Features
  * History views
    * Ranking
    * Raw
    * Favorites
  * History filtering
  * Choosing a command
  * Favorite commands
  * Blacklist
  * Delete last command
* Examples
  

## Features
HSTR is typically started by pressing <kbd>Ctrl</kbd><kbd>r</kbd> after
you configure it for this shorcut for your shell. However,
you can run it as a normal program just by typing:

```bash
hh
```

It opens a page with a history view (ranking view is default).
Alternatively you can run `hh` in non-interactive mode - 
commands are just listed on standard output based on history
view:

```bash
hh --non-interactive
```

Filtering pattern can be optionally specified as well:

```bash
hh -i git
```

Prints history items containing `git` to standard output and
exits.

### History Filtering
When HSTR starts in interative mode, a view of commands
is shown. This list of commands can be filtered just by typing
a string pattern.


### Choosing a Command
When you filtered the view of history items enough, you can
move around it with `UP` and `DOWN` arrow keys. Selected history
items is highlighted. Once you are done with your choice:

* press `ENTER` to choose the command and execute it
* or press `TAB` or `RIGHT` arrow key to choose the command and edit it on command line after exiting `hh`
* or press `LEFT` arrow key to choose the command and open it in editor (Bash `fc` i.e. fix command)


### Favorite Commands
HSTR allows you to store and manage your favorite 
commands. 

A new favorite command can be added from
ranking or raw history view by pressing <kbd>Ctrl</kbd><kbd>f</kbd>.

You can check your favorite commands by choosing 
favorite view - rotate views using <kbd>Ctrl</kbd><kbd>/</kbd> or start
`hh` by adding `favorites` to `HH_CONFIG` environment
property. A favorite command can be choosen just
by pressing `ENTER` when on command in favorite view.
Last chosen favorite commands appears as the first
one (at the top of the page) in the favorite view.
You can delete a favorite command with `DEL` key.

Tips:

* Favorite commands are stored in `~/.hh_favorites`
* Suffix your favorite commands with comments
  describing their purpose. For example:

```
printf "\e[?2004l"   # fix terminal copy/paste
```

* Such comment can be used for normal commands
  as well and may serve as a way how to **tag**
  commands.

```bash
COMMAND # TAG1 TAG2 TAG3
...
printf "\e[?2004l" # fix broken copy/paste in terminal #TERM
```
### Blacklist
HSTR allows you to specify a set of commands to be
skipped from all the views. Blacklist typically contains
frequently used commands whose completion from history 
has a little or no value. The default blacklist looks 
like this:

```bash
pwd
cd
cd ..
ls
hh
mc
```

Tips:

* Blacklist of commands is stored in `~/.hh_blacklist`
  If the file doesn't exist, you may create it and complete
  it with your own blacklist.
* You can skip any command from history just by
  prefixing it with `SPACE`. For example:
  ` echo "Skip this from history"` It's a Bash
  option that is configured using 
  `HISTCONTROL=ignorespace` environment variable.


### Kill the Last Command
Using `hh` you can easily delete the last command from history
e.g. when you make a typo or write something sensitive:

```
hh --kill-last-command
```

See configuration section for how to bind this functionality to
a key - bound to <kbd>Ctrl</kbd><kbd>x</kbd> <kbd>k</kbd> by default.


## Examples
Get more colors when running `hh`:

```bash
export HH_CONFIG=hicolor
```

Start `hh` in configured view and filter out history items 
containing 'git':

```bash
hh git
```

Print history items containing 'git' to standard output and exit:

```bash
hh --non-interactive git
```

Append  default `hh` configuration to your Bash profile:

```bash
hh --show-configuration >> ~/.bashrc
```

Check `hh` man page:

```bash
man hh
```
