#!/bin/bash

files=(01 02 03 04 05 06 07 08 09 12)
sd=(01 02)

#files=(09)
make

for f in "${files[@]}"
do
  printf "data/hd${f}.raw "
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
for f in "${sd[@]}"
do
  printf "data/sd${f}.raw "
  ./huff_codec -c -i "data/sd${f}.raw" -o test.huff -w 128
  ./huff_codec -d -i test.huff -o test2.huff
  diff -q test2.huff "data/sd${f}.raw" >> /dev/null
  if [ $? -eq 0 ]
  then
    echo "OK"
  else
    echo "FAILED"
  fi
done
