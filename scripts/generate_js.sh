#!/bin/sh

for f in "$@"
do
    # Create corresponding JavaScript files
    new_js=`echo "$f" | sed -e 's/\.jpg$/.js/'`
	echo "$new_js"
    echo "// Comment for file $f" > $new_js
    echo "message_english=\"\"" >> $new_js
done
