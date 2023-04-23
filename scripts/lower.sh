for f in $*
do
  new=`echo $f | dd conv=lcase 2>/dev/null`
  echo "$f -> $new"
  mv $f $new
done
