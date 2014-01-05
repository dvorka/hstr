#!/bin/bash

. ./env.sh

cd ..
bzr commit -m "Sync."
bzr push lp:~ultradvorka/+junk/hh-package
cd ..
dput ppa:ultradvorka ${HHRELEASE}_source.changes

# eof
