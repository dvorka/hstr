#!/bin/bash

export HHBZRMSG="Enter first item auto-select, Vim keybinding, doc improvements."
export VERSION=1.15.
export MINOR=1

# precise quantal saucy / precise trusty utopic
for DISTRO in trusty utopic
do

./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"
MINOR=`expr $MINOR + 1`

done

# eof
