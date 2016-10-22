# configure.ac      Autoconf configuration file
#                   process this file with autoconf to produce a configure script
#
# Copyright (C) 2014  Martin Dvorak <martin.dvorak@mindforger.com>
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

AC_PREREQ([2.69])

AC_INIT(hh, 1.21, martin.dvorak@mindforger.com)
AC_CONFIG_FILES([Makefile src/Makefile man/Makefile])

# Checks for src dir existence.
AC_CONFIG_SRCDIR([src/hstr.c])

# Init (no longer takes parameters).
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

# Platform specific ncurses check.
AC_CANONICAL_HOST
PKG_CHECK_MODULES([NCURSES], [ncursesw], [], [
  PKG_CHECK_MODULES([NCURSES], [ncurses], [], [
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
  ])
])

# Checks for libraries.
AC_CHECK_LIB(m, cos, [], [AC_MSG_ERROR([Could not find m library])])
AC_CHECK_LIB(readline, using_history, [], [AC_MSG_ERROR([Could not find readline library])])

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

AC_OUTPUT
