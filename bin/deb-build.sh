#!/bin/bash

rm -vf *.*~

# copy hstr/ source code to a work directory and run this script to create DEB package
cd ..
bzr commit -m "Sync."
bzr builddeb -- -us -uc
bzr builddeb -S && cd ../build-area && pbuilder-dist raring build hh_0.7-0ubuntu1.dsc

# eof
