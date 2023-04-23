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
  convert "$f" -resize 200x200 "$new"

  new=`echo $f | sed 's/\.jpg$/_medium.jpg/'`
  echo "$f -> $new"
  convert "$f" -resize 1020x760 "$new"

  new=`echo $f | sed 's/\.jpg$/_large.jpg/'`
  echo "$f -> $new"
  convert "$f" -resize 1550x1024 "$new"
done
