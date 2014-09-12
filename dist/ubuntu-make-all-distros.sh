#!/bin/bash

export HHBZRMSG="Internatialization and support for transparent terminals."
export VERSION=1.13.
export MINOR=1

# precise quantal saucy trusty
for DISTRO in precise trusty
do

./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"
MINOR=`expr $MINOR + 1`

done

# eof
