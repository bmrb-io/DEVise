#!/bin/sh

#git clean -df
make clean
make setup
make
./js -hostpacu
git clean -df