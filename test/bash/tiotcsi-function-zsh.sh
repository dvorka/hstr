#!/usr/bin/env bash

# ##################################################################
# EXAMPLE: NOT WORKING version

# Define a function to replace a word in the current line
replace-word() {
  local oldword newword
  zle -I # switch to insert mode
  read -k "oldword?Enter word to replace: "
  read -k "newword?Enter replacement word: "
  BUFFER=${BUFFER//$oldword/$newword} # replace old word with new word
  zle redisplay # update the display
}

# Bind the function to a key sequence
bindkey '^Xr' replace-word

# ##################################################################
#
# EXAMPLE: WORKING version w/ foo HSTR

foohstr() {
    echo "command-by-hstr-${1}"
}

hstrnotiocsti() {
  local word
  # we need the WHOLE buffer, not just 0 to cursor: word=${BUFFER[0,CURSOR]}                                                                       
  BUFFER="$(foohstr ${BUFFER})"
  CURSOR=${#BUFFER}
  # update the display                                                                                                                             
  zle redisplay
}

# create ZLE widget ~ readline function                                                                                                            
zle -N hstrnotiocsti
# bind widget to keyboard shortcut                                                                                                                 
bindkey '\C-r' hstrnotiocsti

# ##################################################################
#
# EXAMPLE: WORKING minimal production version w/ foo HSTR
#
# PROBLEM:
# - active ZLE takes over the terminal input & output streams
# - attempt to run HSTR using $(), ``, ... BLOCKS active ZLE progress
# - w/o ZLE it is not possible to insert text into the terminal
#
# SOLUTION:
# - HSTR input: can be enable by reading from </dev/tty
# - HSTR output: is sent to stderr (as stdout is occupied by Curses)

hstr_notiocsti() {
    zle -I
    TMPFILE=$(mktemp)
    </dev/tty hstr ${BUFFER} 2> ${TMPFILE}
    BUFFER="$(cat ${TMPFILE})"
    CURSOR=${#BUFFER}
    zle redisplay
    rm TMPFILE > /dev/null 2>&1
}
zle -N hstr_notiocsti
bindkey '\C-r' hstr_notiocsti

export HSTR_TIOCSTI=n

# ##################################################################
#
# EXAMPLE: WORKING minimal production version w/ foo HSTR
#
# PROBLEM:
# - active ZLE takes over the terminal input & output streams
# - attempt to run HSTR using $(), ``, ... BLOCKS active ZLE progress
# - w/o ZLE it is not possible to insert text into the terminal
#
# SOLUTION:
# - HSTR input: can be enable by reading from </dev/tty
# - HSTR output: we use CUSTOM file handle to get stderr output ONLY
#   1. 2>&1 ... redirect stderr to stdout
#   2. 1>&3 ... redirect stdout to custom file handle
#               (thus ONLY stderr is sent to stdout)
#   3. 3>&- ... close custom file handle
#               (thus close stdout i.e. stdout won't be sent anywhere)
#   { ... } ... to execute in the current shell & set HSTR_OUT
#   3>&1    ... restore stdout to custom file handle

hstr_no_tiocsti() {
    zle -I
    { HSTR_OUT="$( { </dev/tty hstr ${BUFFER}; } 2>&1 1>&3 3>&- )"; } 3>&1;
    BUFFER="${HSTR_OUT}"
    CURSOR=${#BUFFER}
    zle redisplay
}
zle -N hstr_no_tiocsti
bindkey '\C-r' hstr_no_tiocsti

# eof
