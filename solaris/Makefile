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
# Revision 1.22  1997/11/05 00:24:09  donjerko
# *** empty log message ***
#
# Revision 1.21  1997/06/16 16:06:10  donjerko
# New memory management in exec phase. Unidata included.
#
# Revision 1.20  1997/06/11 18:34:48  wenger
# Got xvertext set up to compile on hp, linux, solaris, solsparc, and sun.
#
# Revision 1.19  1997/05/28 15:40:44  wenger
# Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.
#
# Revision 1.18.4.1  1997/05/20 16:11:40  ssl
# Added layout manager to DEVise
#
# Revision 1.18  1997/04/30 21:46:01  wenger
# Fixed non-constant strings in complex mappings bug; TDataAsciiInterp
# no longer gives warning message on blank data lines; added makefile
# targets to make a Purify'd version of multi; fixed uninitialized memory
# read in the DList code; fixed bug that caused 1.4 version of multi to
# always crash; better error messages in DTE command parser; version is
# now 1.4.4.
#
# Revision 1.17  1997/03/28 16:12:31  wenger
# Added headers to all source files that didn't have them; updated
# solaris, solsparc, and hp dependencies.
#

############################################################

#
# Makefile for all components of DEVise
#

LIBMODULES = graphics graphics.new tape issm compustat crsp \
	unidata DTE RTree LM xvertext

#devread is obsolete
#attrproj is broken

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

#	for i in $(LIBMODULES); do echo -- $$i; cd $$i; make all; cd ..; done
libs: date
	$(foreach i, $(LIBMODULES), make -C $i lib &&) echo 

generic: libs
	cd generic; make generic; cd ..

devise: libs
	cd generic; make all; cd ..

multi: libs
	cd multi; make disp; cd ..

attrproj:
	cd attrproj; make all; cd ..

devread:
	cd devread; make all; cd ..

pure: libs
	cd generic; make pure; cd ..

multi.pure: libs
	cd multi; make pure; cd ..
	
static: libs
	cd generic; make static; cd ..

cslib:
	cd graphics; make lib; cd ..
	cd graphics.new; make ClientAPI.o Version.o MapIntToInt.o; cd ..
	cd cslib; make lib; cd ..

# DO NOT DELETE
