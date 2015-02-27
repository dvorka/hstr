#!/bin/bash

export HHBZRMSG="History timestamps, Zsh support and management fixes."
export VERSION=1.16.
export MINOR=1

# precise quantal saucy precise / trusty utopic
for DISTRO in trusty utopic
do

./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"
MINOR=`expr $MINOR + 1`

done

# eof
