#!/bin/sh

make clean
make testclean
git pull
cd src
make clean
make
make jar
cd ..
make
make testsetup
cd cgi-bin
./test_all
cd ..
make install