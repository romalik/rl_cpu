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
cp rootfs/bin/sh rootfs/bstrap
cp rootfs/bin/tar rootfs/bstrap
cp rootfs/scripts/install.sh rootfs/bstrap/init.sh

#-------------------



g++ -o ./tools/make_rootfs ./tools/make_rootfs.cpp && ./tools/make_rootfs hdd 10000000 rootfs/bstrap
