# hstr.pro     Qt project file for HSTR shell history compleation utility
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

#########################################################################
#
# QMake build options:
#   qmake CONFIG+=hstrdebug ... add extra debug and profiling info
#
#########################################################################


TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# -L for where to look for library, -l for linking the library
LIBS += -lm -lreadline -lncursesw -ltinfo

SOURCES += \
    src/hashset.c \
    src/hstr_blacklist.c \
    src/hstr_curses.c \
    src/hstr_favorites.c \
    src/hstr_history.c \
    src/hstr_regexp.c \
    src/hstr_utils.c \
    src/hstr.c \
    src/radixsort.c \
    src/main.c

HEADERS += \
    src/include/hashset.h \
    src/include/hstr_blacklist.h \
    src/include/hstr_curses.h \
    src/include/hstr_favorites.h \
    src/include/hstr_history.h \
    src/include/hstr_regexp.h \
    src/include/hstr_utils.h \
    src/include/radixsort.h \
    src/include/hstr.h

# compiler and linker
hstrdebug {
    QMAKE_CC = ccache gcc -g -pg
} else {
    QMAKE_CC = ccache gcc
}
QMAKE_LINK = gcc
