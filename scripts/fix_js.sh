#!/usr/local/bin/bash

for f in "$@" 
do
    #echo "$f"
    grep "File: " "$f" > /dev/null
    #grep 'function' "$f"
    #echo "$?"
    if [ "$?" == 0 ]; then
	echo "$f"
	echo "// Add comment for file $f inside double-quotes" > "$f"
	echo "message_english=\"\"" >> "$f"
    fi
done
