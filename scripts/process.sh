#!/usr/local/bin/bash

# Your current directory must be root dir of your project

# Copy the root JavaScript file
cp ~/dev/web/scripts/language.js ./

# Change JPEG file names to lower case
# For every JPEG file generate corresponding JavaScript file with a message
find . -name '*.jpg' | xargs ~/dev/web/scripts/lower_text.sh

# For every JPEG file generate preview and medium size JPEG files
find . -name '*.jpg' | xargs ~/dev/web/scripts/resize2.sh

# Generate HTML files overwriting existing files
~/dev/web/htmlGenerator/html_generator.exe -ow
