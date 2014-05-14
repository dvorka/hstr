#!/bin/bash

export NOW=`date +%Y-%m-%d--%H-%M-%S`
export GH_RELEASE_DIR=~/p/hstr/release
export GH_DISTRO_DIR=${GH_RELEASE_DIR}/release-${NOW}

export HHVERSION="1.0.8"
export GH_MSG="GitHub release automation."

# eof
