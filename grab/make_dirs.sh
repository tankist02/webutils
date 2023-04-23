#!/bin/bash

let count=1

for f in "$@"
do
    new=`echo $f | dd bs=2 count=1 2>/dev/null`
    key=`echo $f | dd bs=6 count=1 2>/dev/null`

    if [ ! -d $new ] ; then
        #echo "making dir: $new"
        mkdir $new
    fi

    #echo "$f -> $new"
    cp $f $new

    path=$new"/"$f

    # generate input for postgresql
    echo "$count|$key|$path"

    let count=$count+1
done
