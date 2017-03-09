#!/bin/sh

####################
#  prepare rootfs  #
####################
mkdir -p rootfs
mkdir -p rootfs/bin
mkdir -p rootfs/lib
mkdir -p rootfs/add
rm rootfs/bin/*
rm rootfs/lib/*

cp rootfs/add/* rootfs/bin
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

g++ -o ./tools/make_rootfs ./tools/make_rootfs.cpp && ./tools/make_rootfs hdd 10000000 rootfs/bin
