#!/bin/sh

git clean -df
make setup
make
./js -hostpacu