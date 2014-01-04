#!/bin/bash

export HHVERSION="0.71"
export HH=hh_${HHVERSION}
export HHRELEASE=${HH}-0ubuntu1
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}
export UBUNTUVERSION=raring

# - when asked to change debian/changes, just copy the date and keep 0ubuntu1
# - user email must be as in gpg i.e. (Dvorka) must present
# - hh_ must be with underscore

# eof
