#!/bin/bash

# convert -loop 0 -delay 100 in1.png in2.png out.gif

export FFF=`ls *.png | while read F
do
    echo "${F}"
done`

# delay: 100 > 75
export CCC="convert -loop 0 -delay 75 ${FFF} hstr-v2.gif"

#echo ${CCC};
${CCC}

# eof
