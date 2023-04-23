if [ "$1" = "" ]; then
  echo "Usage: ./doall.sh <file_mask>"
  echo "e.g.: ./doall.sh 'crop????.jpg'"
  exit 1
fi

find -name "$1" | xargs resize.sh
