#!/bin/sh
mkdir -p rootfs
mkdir -p rootfs/bin
mkdir -p rootfs/lib

make -C kernel clean
make -C lib clean
make -C app clean 


make -C kernel
make -C lib
make -C app
g++ -o ./tools/make_rootfs ./tools/make_rootfs.cpp && ./tools/make_rootfs hdd 10000000 rootfs/bin
