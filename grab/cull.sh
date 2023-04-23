#!/bin/bash

# Culls 'good' shtml files - the ones that have corresponding jpeg

mkdir good > /dev/null 2>&1

#for f in `find . -name '??????.jpg' -execdir basename '{}' .jpg \;`
for f in "$@"
do
    #echo $f
    f=`basename $f .jpg`
    #echo $f
    shtml=$f.shtml
    if [ -f $shtml ] ; then
        echo "$shtml -> good"
        cp $shtml good
    fi
done
