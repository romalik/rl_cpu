#!/bin/sh

cat $1 | sed "s/\^/*@/g" | cpp -I"../assembler/include/" - 2>/dev/null | ../assembler/asm -s /dev/stdin $2 $3 $4 $5 $6 $7 $8 $9 ${10} ${11} ${12} | tail -n +1
