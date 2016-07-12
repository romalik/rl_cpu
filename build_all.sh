#!/bin/sh

cd binutils && ./build.sh && ./install.sh && cd ..
cd lcc && mkdir -p build && make all && ./install.sh && cd ..
cd sim && ./build.sh && ./install.sh && cd ..
cd rl_os && ./build.sh
