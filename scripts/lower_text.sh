#!/usr/local/bin/bash

for f in $*
do

    #echo "file: $f"

    # Convert names to lower case
    #new=`echo $f | dd conv=lcase 2>/dev/null`
    #echo "$f -> $new"
    #mv $f $new
    #mv $f temporary_file.jpg
    #mv temporary_file.jpg $new

    # Create corresponding JavaScript files
    #new_js=${new%.jpg}.js
    new_js=${f%.jpg}.js
    echo "// Comment for file $f" > $new_js
    echo "message_english=\"File: $f\"" >> $new_js

done
