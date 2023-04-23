#!/bin/sh

# Create JavaScript files corresponding to base JPG names

for f in `find . -name dsc_\?\?\?\?.jpg`
do
    # Create corresponding JavaScript files
    new_js=`echo "$f" | sed -e 's/\.jpg$/.js/'`
    if [ -f "$new_js" ] ; then
        echo "Skipping existing $new_js"
    else
        echo "$new_js"
        echo "// Comment for file $f" > $new_js
        echo "message_english=\"\"" >> $new_js
        echo "message_russian=\"\"" >> $new_js
    fi
done
