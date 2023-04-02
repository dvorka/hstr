#!/usr/bin/env bash
# This script is used to experiment with Bash functions in order to set 
# the content of the terminal prompt without TIOCSTI ioctl() call.
#
# IMPORTANT: to use the functions below @ Bash
#
# - check the bind command at the end of the file
#   in order to control which function to use
# - source this file in the current shell
#   . ./tiotcsi-functions.h
# - use ^R to test the function

# ORIGINAL FUNCTION VERSION
#
# - function contributed by a HSTR user for Cygwin
# - Bash only (does not work in Zsh)
# - I don't understand {hstrout} trick (actually I do, however, it seems to work in non-interactive mode only)
function hstrcygwin {
  offset=${READLINE_POINT}
  READLINE_POINT=0
  { READLINE_LINE=$(</dev/tty hstr ${READLINE_LINE:0:offset} 2>&1 1>&$hstrout); } {hstrout}>&1
  READLINE_POINT=${#READLINE_LINE}
}

# MY FUNCTION VERSION
#
# - hstr gets current prompt string using READLINE_LINE
# - function sets what to show by changing the content of READLINE_LINE
# - ... which is much simpler than the original function, but is it good enough?

function foohstr {
    echo "hstr-cmd-out ${1}"
}

function mysimple {
    # prompt command content BEFORE
    PROMPT_STR_BEFORE=${READLINE_LINE}
    # cursor position at prompt BEFORE
    PROMPT_LNG_BEFORE=${READLINE_POINT}

    # HSTR gets PROMPT_STR_BEFORE
    # HSTR prints out command which user selected after using ^
    # ^ is set to READLINE_LINE
    # prompt cursor is set to the end of line by setting READLINE_POINT to 1000 (blindly)
    
    { READLINE_LINE=$(foohstr ${PROMPT_STR_BEFORE}) 2>&1; }
    # move cursor to the end of prompt
    READLINE_POINT=${#READLINE_LINE}
}


# EXPERIMENTAL LAB
#
# - understanding bash-fu

function hstrdebug {
    # content of the current terminal prompt
    echo "Readline line : '${READLINE_LINE}'"
    # how many characters is @ current terminal prompt
    echo "Readline point: '${READLINE_POINT}'"

    READLINE_POINT=0
    
    # OBSERVATIONS:
    #   {hstrout}>&1   ... is VALID expression
    #   {hstrout} > &1 ... is INVALID expression

    # command:
    # - runs $() and redirects 2 and 1 to STDOUT
    #{ READLINE_LINE=$(echo COMMAND) 2>&1 1>&$hstrout; } {hstrout}>&1
    # - < /dev/tty
    #   ... read what is @ current prompt from /dev/tty (but I could use READLINE_LINE for it!)
    # - offset=${READLINE_POINT}
    #   ... how long is the string @ prompt
    # - ${READLINE_LINE:0:offset}
    #   ... substring of prompt from 0 to cursor position
    # { READLINE_LINE=$(< /dev/tty echo ${READLINE_LINE:0:offset}) 2>&1; }

    # store read line lenghth to read line POINT to set curser to the end of line
    READLINE_POINT=${#READLINE_LINE}
    # in principle I can do and it will have the same effect
    #   READLINE_POINT=1000

    # after:
    echo "Readline line : '${READLINE_LINE}'"
    echo "Readline point: '${READLINE_POINT}'"
}

#
# BINDING
#

# bind '"\C-r": "\C-a hstr -- \C-j"'
# - bind   
#   ... Bash command which binds key sequence (Ctrl-r) to a command,
#       bind INSERTS the command to the terminal
# - "\C-r"
#   ... key sequence to bind
# - "\C-a hstr -- \C-j"
#   ... command to execute ~ text to be inserted to the terminal:
#       \C-a    ... moves cursor to the beginning of line
#       hstr -- ... inserts "hstr -- " to the terminal
#       \C-j    ... RUNs the command by INSERTing new line
#                   (without Ctrl-j, text would be just inserted, but not run)

# bind -x '"\C-r": "hstrfunction"'
#   ... Bash command which binds key sequence (Ctrl-r) to a command,
#       bind INSERTS the command to the terminal
# - -x
#   ... enables READLINE_LINE and READLINE_POINT
# - hstrfunction
#   ... function which uses READLINE_LINE and READLINE_POINT variables
#

#
# IMPORTANT bind -x
#
# - bind -x must be used in order to enable READLINE_LINE, READLINE_POINT
# - see also: https://www.gnu.org/savannah-checkouts/gnu/bash/manual/bash.html#index-READLINE_005fLINE
#
# READLINE_LINE
# The contents of the Readline line buffer, for use with ‘bind -x’ (see Bash Builtin Commands).
#
# READLINE_POINT
# The position of the insertion point in the Readline line buffer, for use with ‘bind -x’ (see Bash Builtin Commands).

if [[ $- =~ .*i.* ]]; then bind -x '"\C-r": "mysimple"'; fi
#if [[ $- =~ .*i.* ]]; then bind -x '"\C-r": "hstrdebug"'; fi
#if [[ $- =~ .*i.* ]]; then bind -x '"\C-r": "hstrcygwin"'; fi

# ##################################################################
#
# EXAMPLE: WORKING insert into prompt
#

function mytest {
    READLINE_LINE="XYZ"
    READLINE_POINT=3
}
bind -x '"\C-r": "mytest"'

# ##################################################################
#
# EXAMPLE: WORKING foo HSTR w/ a lot of debugs
#
# get terminal input > send it to HSTR > get command from HSTR > insert it

function foohstr {
    echo "CMD_BY_HSTR: >>>${1}<<<"
}

function hstrdebug {
    # content of the current terminal prompt
    echo "Readline line : '${READLINE_LINE}'"
    # how many characters is @ current terminal prompt
    echo "Readline point: '${READLINE_POINT}'"

    TO_SET=$(foohstr ${READLINE_LINE})

    READLINE_POINT=0

    # insert text to terminal
    echo "Setting: '${TO_SET}'"
    READLINE_LINE="${TO_SET}"

    # store read line length to read line POINT to set cursor to the end of line
    READLINE_POINT=${#READLINE_LINE}

    # after:
    echo "Readline line : '${READLINE_LINE}'"
    echo "Readline point: '${READLINE_POINT}'"
}

bind -x '"\C-r": "hstrdebug"'

# ##################################################################
#
# EXAMPLE: WORKING minimal production version w/ foo HSTR

function foohstr {
    echo "CMD_BY_HSTR: >>>${@}<<<"
}

function hstrnotiocsti {
    READLINE_LINE="$(foohstr ${READLINE_LINE})"
    READLINE_POINT=${#READLINE_LINE}
}

bind -x '"\C-r": "hstrnotiocsti"'

# eof
