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

# PROBLEM!
# - active ZLE takes over the terminal, therefore HSTR cannot be run
# - w/o ZLE it is not possible to insert text into the terminal

foohstr() {
    echo "command-by-hstr-${1}"
}

hstrnotiocsti() {
  BUFFER="$(foohstr ${BUFFER})"
  CURSOR=${#BUFFER}
  zle redisplay
}
zle -N hstrnotiocsti
bindkey '\C-r' hstrnotiocsti

# eof
