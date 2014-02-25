#!/bin/sh

cat $1 | sed "s/\^/*@/g" | cpp - 2>/dev/null | ../assembler/asm /dev/stdin -$2 | tail -n +1
