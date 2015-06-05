#!/bin/sh

g++ -o asm ./assembler.cpp
g++ -o trans ./translator.cpp
g++ -o link ./linker.cpp

