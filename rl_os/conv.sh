#!/bin/sh
mkdir -p rootfs/text_conv

for i in ./rootfs/text/*; do
echo $i
echo `basename $i`
iconv -t UTF16BE < $i > ./rootfs/text_conv/`basename $i`
done

