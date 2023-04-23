for f in $@
do
  #echo $f
  #./pat2pdf.sh $f $MAX_PAGES
	./pat2tiff.sh $f $MAX_PAGES
done
