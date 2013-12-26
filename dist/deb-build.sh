#!/bin/bash

. ./env.sh

rm -vf *.*~

cd ..
bzr commit -m "Sync."
bzr builddeb -- -us -uc
bzr builddeb -S && cd ../build-area && pbuilder-dist raring build ${HHRELEASE}.dsc

# eof
