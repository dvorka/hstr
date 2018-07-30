#!/bin/bash

for DISTRO in precise quantal saucy trusty utopic
do

sudo pbuilder --create ${DISTRO}

done

# eof
