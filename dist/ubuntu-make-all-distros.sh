#!/bin/bash

# Launchpad release checklist:
# - update version in source code (hstr.c)
# - update version in configure.ac
# - update release message in this file
# - update version in this file
# - check target Ubuntu distributions in this file
# - check minor version in this file
# - run this script from ~/p/hstr/launchpad

export HHBZRMSG="Buffer overflow fix of regexp mode."
export VERSION=1.20.
export MINOR=1

# precise quantal saucy precise utopic / trusty vivid wily
for DISTRO in trusty vivid wily
do

./ubuntu-make-distro.sh ${DISTRO} ${VERSION}${MINOR} "${HHBZRMSG}"
MINOR=`expr $MINOR + 1`

done

# eof
