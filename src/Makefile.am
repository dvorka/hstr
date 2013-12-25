# http://mij.oltrelinux.com/devel/autoconf-automake/
AM_CFLAGS = --pedantic -Wall -std=c99 -O2
AM_LDFLAGS = -lncurses -lreadline -lm

# bin_ installs to bin; hh_ is the binary name
bin_PROGRAMS = hh

hh_SOURCES = 					\
	hashset.c include/hashset.h 		\
	radixsort.c include/radixsort.h 	\
	hstr_history.c include/hstr_history.h 	\
	hstr_utils.c include/hstr_utils.h 	\
	hstr.c 
