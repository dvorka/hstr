# HSTR Tips and Tricks
Tips:

* [Hiding Commands from History](#hiding-commands-from-history)
* [Commands Tagging](#commands-tagging)
* [Favorite Commands](#favorite-commands)


## Hiding Commands from History

You can exclude commands from being saved to your history by adding a leading space before the command. This requires the `HISTCONTROL` variable to be set appropriately.

```bash
# Bash
export HISTCONTROL=ignorespace

# Zsh
setopt HIST_IGNORE_SPACE
```

Now any command starting with a space will not be recorded in your history:

```bash
 secret-command --password=123
```

You can also prevent specific commands from being saved by setting `HISTIGNORE`:

```bash
# Bash
export HISTIGNORE="ls:cd:pwd:exit:date"

# Zsh
export HISTORY_IGNORE="(ls|cd|pwd|exit|date)"
```

## Commands Tagging

You can add comments at the end of your commands to make them easier to find in HSTR. This is especially useful for complex commands with long paths or many options - just tag them with a short, memorable keyword.

For example, instead of typing the full path segments to find this command:

```bash
cd /home/user/projects/github/hstr # HHH
```

You can simply type `HHH` in HSTR to find it instantly.

Here are more examples of commonly used complex commands with tags:

```bash
# Find Docker container by typing "DDD"
docker run -it --rm -v $(pwd):/app -w /app node:18 npm install # DDD

# Find SSH tunnel by typing "SSS"
ssh -L 8080:localhost:3000 user@remote-server.example.com # SSS

# Find database backup by typing "BBB"
pg_dump -h localhost -U postgres -d mydb > backup_$(date +%Y%m%d).sql # BBB

# Find complex Git command by typing "GGG"
git log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr)%Creset' --abbrev-commit # GGG
```

Tags work because HSTR searches the entire command line, including comments. Choose short, memorable tags that are easy to type.


## Favorite Commands

Mark your most frequently used commands as favorites for instant access:

1. **Add to favorites**: Navigate to a command in HSTR and press <kbd>Ctrl-f</kbd>
2. **View favorites**: Press <kbd>Ctrl-/</kbd> to toggle between history and favorites view
3. **Remove from favorites**: Navigate to a favorited command and press <kbd>DEL</kbd> again

Favorites are persistent across sessions and stored in `~/.hstr_favorites` file.


