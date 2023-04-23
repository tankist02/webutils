#!/bin/csh

foreach f ($*)
    set new=`echo $f | sed 's/\?//' | sed 's/JPG/jpg/'`
    echo "$f -> $new"
    mv $f $new
end

