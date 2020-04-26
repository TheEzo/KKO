#!/bin/bash

files=(hd01 hd02 hd03 hd04 hd05 hd06 hd07 hd08 hd09 hd12 sd01 sd02)

make

for f in "${files[@]}"
do
  printf "data/${f}.raw   normal: "
  ./huff_codec -c -i "data/${f}.raw" -o test.huff
  ./huff_codec -d -i test.huff -o test2.huff
  diff -q test2.huff "data/${f}.raw" >> /dev/null
  if [ $? -eq 0 ]
  then
    echo "OK"
  else
    echo "FAILED"
  fi
  rm -f test.huff test2.huff
  printf "data/${f}.raw adaptive: "
  ./huff_codec -c -i "data/${f}.raw" -o test.huff -a
  ./huff_codec -d -i test.huff -o test2.huff -a
  diff -q test2.huff "data/${f}.raw" >> /dev/null
  if [ $? -eq 0 ]
  then
    echo "OK"
  else
    echo "FAILED"
  fi
  rm -f test.huff test2.huff
done

