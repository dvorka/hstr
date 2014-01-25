#!/bin/bash

# Debian release: https://wiki.debian.org/IntroDebianPackaging

export HHVERSION="1.0.2"
export HHFULLVERSION=${HHVERSION}
export HH=hh_${HHVERSION}
export HHRELEASE=hh-${HHFULLVERSION}
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}

export UBUNTUVERSION=UNRELEASED

export HHBZRMSG="Stabilization, fixes and better BASH shortcuts compatibility."

# eof
