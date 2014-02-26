#!/bin/sh

cat $1 | sed "s/\^/*@/g" | cpp -I"../assembler/include/" - 2>/dev/null | ../assembler/asm /dev/stdin -$2 | tail -n +1
