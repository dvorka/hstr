#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#

AC_PREREQ([2.69])

AC_INIT(hh, 1.13, martin.dvorak@mindforger.com)
AM_INIT_AUTOMAKE(hh, 1.13)
AC_CONFIG_FILES([Makefile src/Makefile man/Makefile])

# Checks for src dir existence.
AC_CONFIG_SRCDIR([src/hstr.c])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(ncursesw, killwchar, [], [AC_MSG_ERROR([Could  not find ncursesw library])])
AC_CHECK_LIB(m, cos, [], [AC_MSG_ERROR([Could not find m library])])
AC_CHECK_LIB(readline, using_history, [], [AC_MSG_ERROR([Could not find readline library])])

# Checks for header files.
AC_CHECK_HEADER(assert.h)
AC_CHECK_HEADER(ctype.h)
AC_CHECK_HEADER(fcntl.h)
AC_CHECK_HEADER(getopt.h)
AC_CHECK_HEADER(locale.h)
AC_CHECK_HEADER(math.h)
AC_CHECK_HEADER(ncursesw/curses.h)
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
