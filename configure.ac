# configure.ac      Autoconf configuration file
#
# Copyright (C) 2014-2020  Martin Dvorak <martin.dvorak@mindforger.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ([2.69])

AC_INIT(hstr, 2.2.0, martin.dvorak@mindforger.com)
AC_CONFIG_FILES([Makefile src/Makefile man/Makefile])

# Check src dir existence.
AC_CONFIG_SRCDIR([src/hstr.c])

# Init (no longer takes parameters).
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# NCURSES check w/o PKG_CHECK_MODULES macro: CHECK_LIB > MODULES > CHECK_LIB
# Determine OS
AC_CANONICAL_HOST

AC_CHECK_PROG(PKG_CONFIG_EXISTS,[pkg-config],[pkg-config],[no])
if test "$PKG_CONFIG_EXISTS" = "no"
then
    # Notify user that pkg-tools are required
    AC_MSG_NOTICE([===================================================================================])
    AC_MSG_NOTICE([IMPORTANT: Make sure you have pkg-config installed - it's needed to run this script])
    AC_MSG_NOTICE([===================================================================================])
    AC_MSG_ERROR([Please install required program 'pkg-config' and run build/tarball/tarball-automake.sh again.])
fi

# PKG_CHECK_MODULES macro is NOT used to avoid confusing syntax errors in case that pkg-config is NOT installed
AC_CHECK_LIB(ncursesw, killwchar, [],
[
    AC_CHECK_LIB(ncurses, killwchar, [], 
    [
        AC_SUBST([NCURSESW_CFLAGS])
        AC_SUBST([NCURSESW_LIBS])
        if pkg-config --exists ncursesw
        then
            AC_MSG_NOTICE([Module ncursesw found])
            NCURSESW_CFLAGS=`pkg-config --cflags ncursesw`
            NCURSESW_LIBS=`pkg-config --libs ncursesw`
        else
            if pkg-config --exists ncurses
            then
                AC_MSG_NOTICE([Module ncurses found])
                NCURSESW_CFLAGS=`pkg-config --cflags ncursesw`
                NCURSESW_LIBS=`pkg-config --libs ncurses`
            else
                AS_CASE([$host_os],
                  [darwin*],
                    [
                      AC_CHECK_LIB(ncurses, killwchar, [], [AC_MSG_ERROR([Could not find ncurses library])])
                      AC_CHECK_HEADER(curses.h)
                    ],
                  [
                    AC_CHECK_LIB(ncursesw, killwchar, [], [AC_MSG_ERROR([Could not find ncursesw library])])
                    AC_CHECK_HEADER(ncursesw/curses.h)
                  ]
                )
            fi
        fi
    ]) # FAIL of ncurses
]) # FAIL of ncursesw


# Checks for libraries.
AC_CHECK_LIB(m, cos, [], [AC_MSG_ERROR([Could not find m library])])
AC_CHECK_LIB(readline, using_history, [], [AC_MSG_ERROR([Could not find readline library])])
# ncurses might be linked in libtinfo
#AC_CHECK_LIB(tinfo, keypad, [], [AC_MSG_ERROR([Could not find tinfo library])])

# Checks for header files.
AC_CHECK_HEADER(assert.h)
AC_CHECK_HEADER(ctype.h)
AC_CHECK_HEADER(fcntl.h)
AC_CHECK_HEADER(getopt.h)
AC_CHECK_HEADER(locale.h)
AC_CHECK_HEADER(math.h)
AC_CHECK_HEADER(readline/history.h)
AC_CHECK_HEADER(regex.h)
AC_CHECK_HEADER(signal.h)
AC_CHECK_HEADER(stdbool.h)
AC_CHECK_HEADER(stddef.h)
AC_CHECK_HEADER(stdio.h)
AC_CHECK_HEADER(stdlib.h)
AC_CHECK_HEADER(string.h)
AC_CHECK_HEADER(sys/ioctl.h)
AC_CHECK_HEADER(termios.h)
AC_CHECK_HEADER(unistd.h)
AC_CHECK_HEADER(wchar.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup strstr])

# Bash@Ubuntu@Windows
AC_CHECK_FILE(/tmp/hstr-ms-wsl,AC_DEFINE(__MS_WSL__), [])

# Bash CLI autocomplete
AC_ARG_WITH([bash-completion-dir],
    AS_HELP_STRING([--with-bash-completion-dir[=PATH]],
        [Install the bash auto-completion script in this directory. @<:@default=yes@:>@]),
    [],
    [with_bash_completion_dir=yes])

if test "x$with_bash_completion_dir" = "xyes"; then
    # pkg-config command existence for PKG_CHECK_MODULES macro checked above
    PKG_CHECK_MODULES([BASH_COMPLETION], [bash-completion >= 2.0],
        [BASH_COMPLETION_DIR="`pkg-config --variable=completionsdir bash-completion`"],
        [BASH_COMPLETION_DIR="$datadir/bash-completion/completions"])
else
    BASH_COMPLETION_DIR="$with_bash_completion_dir"
fi

AC_SUBST([BASH_COMPLETION_DIR])
AM_CONDITIONAL([ENABLE_BASH_COMPLETION],[test "x$with_bash_completion_dir" != "xno"])

AC_OUTPUT
