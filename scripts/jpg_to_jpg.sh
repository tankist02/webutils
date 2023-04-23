#!/bin/sh

for f in "$@"
do
    echo "$f"
    convert "$f" -resize 1550x1120 -quality 85 temporary_file
    mv temporary_file "$f"
done

