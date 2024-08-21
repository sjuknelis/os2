#!/bin/zsh

cd $(dirname $0)

gcc main.c fake6502.c -o emu