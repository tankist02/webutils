#!/bin/sh

# Assumes all file names are corrected by fix_file_names.sh

for f in "$@"
do
  #echo "file: $f"

  # Skip previews and mediums
  res=`echo "$f" | egrep '(_preview\.jpg|_medium\.jpg|_large\.jpg)'`
  #echo "res: $res"

  if [ "$res" != "" ] ; then
    echo "Skipping: $f"
    continue
  fi

  new=`echo $f | sed 's/\.jpg$/_preview.jpg/'`
  echo "$f -> $new"
  #convert "$f" -resize 300x300 "$new"
  convert -define jpeg:size=300x300 "$f"  -thumbnail 200x200^ -gravity center -extent 200x200  "$new"
done
