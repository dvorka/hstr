#!/bin/bash

export HHVERSION="1.7.10"
export HHFULLVERSION=${HHVERSION}-0ubuntu1
export HH=hh_${HHVERSION}
export HHRELEASE=hh_${HHFULLVERSION}
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}

#export UBUNTUVERSION=quantal
#export UBUNTUVERSION=raring
export UBUNTUVERSION=saucy

export HHBZRMSG="Making radix sort allocation more effient and inserts more robust."

# - user email must be the same as in gpg i.e. (Dvorka) must present
# - hh_ must be with underscore (dh_make enforced)

# eof
