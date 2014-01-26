#!/bin/bash

# Debian release: https://wiki.debian.org/IntroDebianPackaging

export HHVERSION="1.0.5"
export HHFULLVERSION=${HHVERSION}-1.0
export HH=hh_${HHVERSION}
export HHRELEASE=hh-${HHFULLVERSION}
export HHSRC=/home/dvorka/p/hstr/github/hstr
export NOW=`date +%Y-%m-%d--%H-%M-%S`
export HHBUILD=hstr-${NOW}

export UBUNTUVERSION=unstable

export HHBZRMSG="Making the .deb package compliant with Debian: NMU version, version ..."

# eof
