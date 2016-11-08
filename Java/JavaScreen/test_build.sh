#!/bin/sh

make clean
make setup
make
./js -hostpacu
git clean -df