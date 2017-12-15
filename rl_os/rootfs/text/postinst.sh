#!/bin/sh

mkdir /home
rm /init.sh
rm /sh
#rm /tar
#rm /bin.tar
#rm /text_conv.tar


cp scripts/init.sh /

exec /init.sh

