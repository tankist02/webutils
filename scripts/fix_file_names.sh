#!/bin/sh

# Renames file(s) by substituting non-alphanumeric chars with underscores
# Should be invoked like this:
# find . -type f -print0 | xargs -0 fix_file_names.sh

for f in "$@"
do
	#echo "$f"
	#continue

	dir=`dirname "$f"`
	#echo "dir: $dir"
	base=`echo "$f" | sed 's/.*\///'`
	#echo "base: $base"
	base=`fix_file_name "$base"`
	#echo "base: $base"
	new=$dir"/"$base".jpg"

	if [ "$f" = "$new" ]
	then
		continue
	fi

	if [ -f "$new" ]
	then
		echo "File $new exists, skipping"
	else
		echo "$f -> $new"
		mv "$f" "$new"
	fi
done

