#!/bin/sh

cd cmd
make clean
make
make install
cd ..


cd vt
make clean
make
make install
cd ..


cd serialip
make clean
make
make install
cd ..


#cd linenoise
#make clean
#make
#make install
#cd ..

