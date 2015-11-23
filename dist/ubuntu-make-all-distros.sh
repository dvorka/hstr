#!/bin/bash

# Launchpad release checklist:
# - update version in source code (hstr.c)
# - update version in configure.ac
# - update release message in this file
# - update version in this file
# - check target Ubuntu distributions in this file
# - check minor version in this file
# - run this script from ~/p/hstr/launchpad

export HHBZRMSG="History management and color themes fixes."
export VERSION=1.18.
export MINOR=2

# precise quantal saucy precise / trusty utopic vivid wily
for DISTRO in trusty utopic vivid
do

./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"
MINOR=`expr $MINOR + 1`

done

# eof
