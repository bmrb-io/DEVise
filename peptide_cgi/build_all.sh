#!/bin/sh

# Clean first
cd ..
make clean
make testclean
cd src
make clean

# Now make
make
cd ..
make
make testsetup

cd cgi-bin
./tests/test1