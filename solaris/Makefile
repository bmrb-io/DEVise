#
# Makefile for all components of DEVise
#

MODULES = graphics graphics.new tape issm compustat crsp parser \
	generic attrproj devread multi

LIBMODULES = graphics graphics.new tape issm compustat crsp

all clean mostlyclean:
	for i in $(MODULES); do echo -- $$i; cd $$i; make $@; cd ..; done

libs:
	for i in $(LIBMODULES); do echo -- $$i; cd $$i; make all; cd ..; done

.PHONY: generic devise multi attrproj devread

generic: libs
	cd generic; make generic; cd ..

devise: libs
	cd generic; make all; cd ..

multi: libs
	cd multi; make all; cd ..

attrproj:
	cd attrproj; make all; cd ..

devread:
	cd devread; make all; cd ..

pure: libs
	cd generic; make pure; cd ..
	
static: libs
	cd generic; make static; cd ..
