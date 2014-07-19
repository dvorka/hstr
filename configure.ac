#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ([2.69])

AC_INIT(hh, 0.90, martin.dvorak@mindforger.com)
AM_INIT_AUTOMAKE(hh, 0.90)
AC_OUTPUT(Makefile src/Makefile man/Makefile)

# Checks for src dir existence.
AC_CONFIG_SRCDIR([src/hstr.c])
# Where to generate header file.
#AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(ncursesw, initscr, [], [AC_MSG_ERROR([Could  not find Curses library])])
AC_CHECK_LIB(m, cos, [], [AC_MSG_ERROR([Could  not find Math library])])
AC_CHECK_LIB(readline, using_history, [], [AC_MSG_ERROR([Could  not find Readline library])])

# Checks for header files.
AC_CHECK_HEADER(assert.h)
AC_CHECK_HEADER(ctype.h)
AC_CHECK_HEADER(curses.h)
AC_CHECK_HEADER(fcntl.h)
AC_CHECK_HEADER(math.h)
AC_CHECK_HEADER(readline/history.h)
AC_CHECK_HEADER(stdbool.h)
AC_CHECK_HEADER(stddef.h)
AC_CHECK_HEADER(stdio.h)
AC_CHECK_HEADER(stdlib.h)
AC_CHECK_HEADER(string.h)
AC_CHECK_HEADER(sys/ioctl.h)
AC_CHECK_HEADER(termios.h)
AC_CHECK_HEADER(unistd.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup strstr])

AC_OUTPUT
