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
make -C libc clean
make -C libc
cp -rfv libc/include rootfs/lib
#make -C libc_bsd clean
#make -C libc_bsd
#cp -rfv libc/include rootfs/lib
#-------------------



####################
#  prepare apps    #
####################
cd app
./build.sh
cd ..

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
rm -rf rootfs/hda
mkdir -p rootfs/hda
cd rootfs
rm -rf scripts
cp -rfv text_conv scripts
tar cfv hda/bin.tar bin
tar cfv hda/scripts.tar scripts
cd ..
cp rootfs/bin/sh rootfs/hda
cp rootfs/bin/mount rootfs/hda
cp rootfs/bin/mkdir rootfs/hda
cp rootfs/bin/mknod rootfs/hda
cp rootfs/bin/umount rootfs/hda
cp rootfs/bin/cat rootfs/hda
#cp rootfs/bin/wc rootfs/hda
cp rootfs/bin/echo rootfs/hda
#cp rootfs/bin/nctest rootfs/hda
#cp rootfs/bin/test rootfs/hda
cp rootfs/bin/ls rootfs/hda
cp rootfs/bin/ps rootfs/hda
cp rootfs/bin/dd rootfs/hda
#cp rootfs/bin/hello rootfs/hda
#cp rootfs/bin/kill rootfs/hda
#cp rootfs/bin/cat rootfs/hda
cp rootfs/bin/tar rootfs/hda
#cp rootfs/bin/testrpc rootfs/hda
cp rootfs/bin/mt_test rootfs/hda
cp rootfs/bin/setloop rootfs/hda
#cp rootfs/bin/vt rootfs/hda

cp rootfs/scripts/install.sh rootfs/hda/
cp rootfs/scripts/st.sh rootfs/hda/
cp hdb_s.img ./rootfs/hda
#-------------------



g++ -o ./tools/make_rootfs ./tools/make_rootfs.cpp
./tools/make_rootfs hda 10000000 rootfs/hda
./tools/make_rootfs hdb 20000 rootfs/hdb
