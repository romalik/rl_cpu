#!/bin/sh

####################
#  prepare rootfs  #
####################
mkdir -p rootfs
mkdir -p rootfs/bin
mkdir -p rootfs/lib
rm rootfs/bin/*
rm rootfs/lib/*

./conv.sh
#-------------------


####################
#  prepare kernel  #
####################
make -C kernel clean
make -C kernel
#-------------------


####################
#  prepare libc    #
####################
#make -C lib clean
#make -C lib
#cp -rfv lib/include rootfs/lib
make -C libc clean
make -C libc
cp -rfv libc/include rootfs/lib
#-------------------



####################
#  prepare apps    #
####################
make -C app clean 
make -C app
#-------------------



####################
#  build lcc       #
####################
#cd app/lcc-strip
#make clean
#make rcc
#cd ../..
#cp app/lcc-strip/build/rcc rootfs/bin
#-------------------


####################
#  prepare arch    #
####################
rm -rf rootfs/bstrap
mkdir -p rootfs/bstrap
cd rootfs
rm -rf scripts
cp -rfv text_conv scripts
tar cfv bstrap/bin.tar bin
tar cfv bstrap/scripts.tar scripts
cd ..
cp ls_c rootfs/bstrap
cp rootfs/bin/sh rootfs/bstrap
cp rootfs/bin/cat rootfs/bstrap
cp rootfs/bin/wc rootfs/bstrap
cp rootfs/bin/echo rootfs/bstrap
#cp rootfs/bin/test rootfs/bstrap
cp rootfs/bin/ls rootfs/bstrap
cp rootfs/bin/ps rootfs/bstrap
#cp rootfs/bin/dd rootfs/bstrap
#cp rootfs/bin/hello rootfs/bstrap
#cp rootfs/bin/kill rootfs/bstrap
#cp rootfs/bin/cat rootfs/bstrap
cp rootfs/bin/tar rootfs/bstrap
cp rootfs/bin/vt rootfs/bstrap
cp rootfs/bin/reader rootfs/bstrap
cp levee rootfs/bstrap/

cp rootfs/scripts/install.sh rootfs/bstrap/init.sh

#-------------------



g++ -o ./tools/make_rootfs ./tools/make_rootfs.cpp && ./tools/make_rootfs hdd 10000000 rootfs/bstrap
