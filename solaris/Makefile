# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1997
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# $Id$

# $Log$

############################################################

#
# Makefile for all components of DEVise
#

LIBMODULES = graphics graphics.new tape issm compustat crsp \
	attrproj devread DTE 

MODULES = $(LIBMODULES) parser generic multi

.PHONY: clean mostlyclean date libs generic devise multi attrproj devread \
	cslib

all: date
	for i in $(MODULES); do echo -- $$i; cd $$i; make $@; cd ..; done

clean mostlyclean:
	for i in $(MODULES); do echo -- $$i; cd $$i; make $@; cd ..; done
	cd cslib; make $@

date:
	cd generic; make CompDate.o

libs: date
	for i in $(LIBMODULES); do echo -- $$i; cd $$i; make all; cd ..; done

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

cslib:
	cd graphics; make lib; cd ..
	cd graphics.new; make ClientAPI.o Version.o MapIntToInt.o; cd ..
	cd cslib; make lib; cd ..

# DO NOT DELETE
