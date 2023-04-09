#!/bin/bash

# group commands in a sub-shell: ( )
#  (list)
# group commands in the CURRENT shell: { }
#  { list; }
# test - return the binary result of an expression: [[ ]]
#  [[ expression ]]
# arithmetic expansion
#  $(( expression ))
#  (( expression ))

# { can be used to GROUP output of multiple commands into one BLOB
#  - https://www.linux.com/topic/desktop/all-about-curly-braces-bash/
#  - ; to separate commands
#  - executed in the curren shell
{ echo a; echo B; echo c; }
echo "-------------------"

# string operations @ var
MY_VAR="abcde"
echo ${MY_VAR:0:1} # a
echo ${MY_VAR:0:2} # ab
echo ${MY_VAR:0:3} # abc
echo "-------------------"

# new and suggested execution 
MY_VAR="$(pwd)"
echo $MY_VAR


# Bash bind:
#
# bash -x keyseq:shell-command
#   Cause SHELL-COMMAND to be executed when KEYSEQ is entered
#
# Bash VARIABLES doc:
#
# READLINE_LINE
#   The contents of the Readline line buffer, for use with ‘bind -x’
# READLINE_POINT
#   The position of the insertion point in the Readline line buffer, for use with ‘bind -x’
#

echo "Readline line : '${READLINE_LINE}'"
echo "Readline point: '${READLINE_POINT}'"

# TODO
# TODO
# TODO
# TODO ^ must be tested @ .bashrc @ bind -x where these variable are set (not set elsewhere)
# TODO
# TODO

echo "- expression: ------------------"

# TODO
# execute COMMAND in the CURRENT shell and store it to variable READLINE_LINE
#{ READLINE_LINE=$(echo COMMAND) 2>&1 1>&$hstrout; } {hstrout} > &1

echo "^------------------"

#
# DECODING CRAZY EXPRESSION
#
# { READLINE_LINE=$(</dev/tty hstr ${READLINE_LINE:0:offset} 2>&1 1>&$hstrout); } {hstrout}>&1
echo "- expression 1: ------------------"
# execute COMMAND in the CURRENT shell and store it to variable READLINE_LINE
{ READLINE_LINE=$(echo COMMAND); }
echo $READLINE_LINE
echo "^------------------"

# read input for the command from file
grep abc < without-tiotcsh.sh
# EQUIVALENT way how to write it (reads input for command from file):
< without-tiotcsh.sh grep abc

echo "- expression 2: ------------------"
# execute COMMAND in the CURRENT shell and store it to variable READLINE_LINE

# {hstrout} >&1
#   ... run command which is in the hstrout
function myfun {
    # { READLINE_LINE=$(echo COMMAND) 2>&1 1>&$hstrout; } {hstrout} >&1
    hstrout=ls
    { READLINE_LINE=$(echo COMMAND) 2>&1 1>&$hstrout; } {hstrout}
}
myfun
echo "^------------------"

# CMD > &${hstrout}
#   ... redirect standard output of the command CMD to the FILE whose name is stored in hstrout variable

################################################################################
# zsh version by fzf
#
# https://github.com/junegunn/fzf/blob/c387689d1cd45f0d8eb122fe95ee72ccc61d3bff/shell/key-bindings.bash#L41


# eof
