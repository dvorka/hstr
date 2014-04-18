#!/bin/bash

export DISTRO=trusty

sudo pbuilder --create $DISTRO
cp /var/cache/pbuilder/base.tgz ~/pbuilder/${DISTRO}-base.tgz


# eof
