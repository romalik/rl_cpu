#!/bin/sh

echo "v2.0 raw" > ls.$1.0
echo "v2.0 raw" > ls.$1.8
echo "v2.0 raw" > ls.$1.16

cat $1.0 >> ls.$1.0
cat $1.8 >> ls.$1.8
cat $1.16 >> ls.$1.16

