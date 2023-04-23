for f in $*
do
  echo "file: $f"
  res=`echo $f | egrep '(_preview\.jpg|_medium\.jpg)'`
  #echo "res: $res"
  if [ "$res" != "" ] ; then
    echo "skipping"
    continue
  fi

done
