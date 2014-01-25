#!/bin/bash
 
. ./ubuntu-env.sh

cd ..
#bzr commit -m "Sync."
bzr push lp:~ultradvorka/+junk/hh-package
cd ..
#rm -rvf *.orig.tar.gz ./build-area/*.orig.tar.gz
dput ppa:ultradvorka ${HHRELEASE}_source.changes

# eof
