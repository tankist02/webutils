#!/bin/sh

# Renames file(s) to lower case

for f in "$@"
do
    # If directory part is empty it becomes "."
    dir=`dirname "$f"`
    base=`echo "$f" | sed 's/.*\///' | dd conv=lcase 2>/dev/null`

    new=$dir"/"$base

    echo "$f -> $new"
done

