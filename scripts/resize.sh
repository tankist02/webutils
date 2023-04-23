for f in $*
do
  new=${f%.jpg}_preview.jpg
  echo "$f -> $new"
  djpeg -scale 1/4 "$f" | cjpeg > "$new"
done
