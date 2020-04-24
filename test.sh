#!/bin/bash

files=(01 02 03 07 08 09 12)
#files=(01)
make

for f in "${files[@]}"
do
  echo "data/hd${f}.raw"
  ./huff_codec -c -i "data/hd${f}.raw" -o test.huff -w 512
  ./huff_codec -d -i test.huff -o test2.huff
  diff -q test2.huff "data/hd${f}.raw" >> /dev/null
  if [ $? -eq 0 ]
  then
    echo "OK"
  else
    echo "FAILED"
  fi
done
