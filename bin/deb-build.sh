#!/bin/bash

rm -vf *.*~

cd ..
bzr commit -m "Sync."
bzr builddeb -- -us -uc
bzr builddeb -S && cd ../build-area && pbuilder-dist raring build hh_0.7-0ubuntu1.dsc

# eof
