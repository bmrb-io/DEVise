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
# Revision 1.33  1998/02/09 17:26:06  wenger
# Removed multi from default compile and Solaris release script.
#
# Revision 1.32  1998/01/30 02:17:25  wenger
# Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.
#
# Revision 1.31  1998/01/23 20:38:25  zhenhai
# *** empty log message ***
#
# Revision 1.30  1998/01/21 17:47:21  wenger
# Switched csgroup back to cslib pending Dongbin's commits of csgroup.
#
# Revision 1.29.2.1  1998/02/12 05:12:33  taodb
# Allowed make to clean the csgroup binary files
#
# Revision 1.29  1998/01/15 19:45:48  taodb
# Replaced cslib with csgroup
#
# Revision 1.28  1998/01/14 22:25:16  wenger
# Improved 'strip' target.
#
# Revision 1.27  1998/01/07 19:32:07  wenger
# Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
# server library into Devise); updated solaris, sun, linux, and hp
# dependencies.
#
# Revision 1.26  1997/12/23 21:00:19  wenger
# Got the latest version of DEVise to compile and link on SPARC/Solaris;
# removed (old) layout manager code from DEVise link.
#
# Revision 1.25  1997/12/12 16:37:53  wenger
# Added 'strip' target.
#
# Revision 1.24  1997/12/02 17:35:38  wenger
# Set up src/color and src/utils to compile "normally" for solaris and
# solsparc.
#
# Revision 1.23  1997/11/14 00:34:56  donjerko
# Fixed the multi target.
#
# Revision 1.22  1997/11/05 00:24:09  donjerko
# *** empty log message ***
#
# Revision 1.21.6.2  1998/01/16 23:42:06  wenger
# Fixed some problems that Tony found with the client/server communication
# and GIF generation; fixed problem that session files specified on the
# command line were still opened by the Tcl code.
#
# Revision 1.21.6.1  1997/12/23 21:43:31  wenger
# Disabled layout manager GUI and removed (old) layout manager code
# from DEVise link.
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
	unidata DTE RTree xvertext color utils csgroup 

#devread is obsolete
#attrproj is broken

MODULES = $(LIBMODULES) parser generic cslib
#MODULES = $(LIBMODULES) parser generic multi cslib

.PHONY: clean mostlyclean date libs generic devise multi attrproj devread \
	csgroup

all: date
	for i in $(MODULES); do echo -- $$i; cd $$i; make $@; cd ..; done

clean mostlyclean:
	for i in $(MODULES); do echo -- $$i; cd $$i; make $@; cd ..; done

date:
	cd generic; make CompDate.o

#	for i in $(LIBMODULES); do echo -- $$i; cd $$i; make all; cd ..; done
libs: date
	$(foreach i, $(LIBMODULES), make -C $i lib &&) echo 

generic: libs
	cd generic; make generic; cd ..

devised: libs
	cd generic; make devised; cd ..

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

strip:
	- strip generic/generic generic/generic.static
	- strip generic/devise generic/deviseb generic/devised
	- strip multi/disp

# DO NOT DELETE
