#!/bin/csh

foreach f ($*)
    set new=`echo $f | sed 's/ppm$/jpg/'`
    echo "$f -> $new"
    convert "$f" -resize 1550x1120 -quality 85 -unsharp 0.9x0.9+1.2 $new
end

