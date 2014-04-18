#!/bin/bash

for DISTRO in precise quantal saucy trusty
do

sudo pbuilder --create ${DISTRO}

done

# eof
