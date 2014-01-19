#!/bin/bash

export HHVERSION="0.92"
export HHFULLVERSION=${HHVERSION}-0ubuntu1
export HH=hh_${HHVERSION}
export HHRELEASE=hh_${HHFULLVERSION}
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}

#export UBUNTUVERSION=quantal
#export UBUNTUVERSION=raring
export UBUNTUVERSION=saucy

export HHBZRMSG="Fixed keyboard loop; Ctrl-r consistent shortcuts; propagation of the text from the prompt to HH."

# - edit config.am ... set new version
# - user email must be as in gpg i.e. (Dvorka) must present
# - hh_ must be with underscore

# eof
