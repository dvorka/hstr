#!/bin/bash

# boostrap new OR refresh distribution base for pbuilder
export DISTRO=zesty

sudo pbuilder --create $DISTRO
rm -vf ~/pbuilder/${DISTRO}-base.tgz
cp /var/cache/pbuilder/base.tgz ~/pbuilder/${DISTRO}-base.tgz

# eof
