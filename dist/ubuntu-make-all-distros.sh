#!/bin/bash

export HHBZRMSG="Favorites stabilization: deletion, reordering and highlighting."
export VERSION=1.11.
export MINOR=1

for DISTRO in precise quantal saucy trusty
do

MINOR=`expr $MINOR + 1`

./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"

done

# eof
