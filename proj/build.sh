#!/bin/zsh

cd $(dirname $0)

IFS=$'\n' c_files=($(find . -name "*.c"))
for c_file in $c_files; do
    cc65 -t none -O --cpu 6502 $c_file
done

IFS=$'\n' s_files=($(find . -name "*.s"))
for s_file in $s_files; do
    ca65 --cpu 6502 $s_file
done

IFS=$'\n' o_files=($(find . -name "*.o"))
ld65 -C cpu/sbc.cfg -m proj.map -o proj.out $o_files cpu/sbc.lib