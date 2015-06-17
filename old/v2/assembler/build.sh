#!/bin/sh
cpp $1 | gsed "s/__CR__/\n/g" > __tmp.s
../assembler/asm __tmp.s
rm ./__tmp.s
