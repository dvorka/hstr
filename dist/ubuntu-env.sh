#!/bin/bash

export HHVERSION="1.9.10"
export HHFULLVERSION=${HHVERSION}-0ubuntu1
export HH=hh_${HHVERSION}
export HHRELEASE=hh_${HHFULLVERSION}
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}

## https://wiki.ubuntu.com/Releases
#export UBUNTUVERSION=precise
#export UBUNTUVERSION=quantal
#export UBUNTUVERSION=saucy
export UBUNTUVERSION=trusty

export HHBZRMSG="Favorites - favorite commands can be bookmarked and shown in a new view!"

# - user email must be the same as in gpg i.e. (Dvorka) must present
# - hh_ must be with underscore (dh_make enforced)

# eof
