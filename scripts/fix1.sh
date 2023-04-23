#!/bin/csh

foreach f ($*)
    #set new=`echo $f | sed 's/JPG/jpg/'`
    set new=`echo $f | sed 's/DSCN/dscn/'`
    echo "$f -> $new"
    mv $f $new
end

