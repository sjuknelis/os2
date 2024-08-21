#!/bin/zsh

cd $(dirname $0)

./proj/build.sh
./emu/build.sh
./emu/emu