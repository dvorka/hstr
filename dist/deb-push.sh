#!/bin/bash

. ./env.sh

cd ../..
bzr push lp:~ultradvorka/+junk/hh-package
dput ppa:ultradvorka ${HHRELEASE}_source.changes

# eof