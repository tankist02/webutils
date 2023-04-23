for f in $*
do
  new=`echo $f | dd conv=ucase 2>/dev/null`
  echo "$f -> $new"
  mv $f $new
done
