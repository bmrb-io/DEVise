#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1995
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

#  $Id$

#  $Log$
#  Revision 1.1  1996/12/06 19:00:36  wenger
#  Fixed up linux dirs so latest version compiles for Linux; updated
#  Linux dependencies.
#

############################################################

ARCH_FLAGS = -DLINUX -I/usr/include/tcl

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/
include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.linux

dte: $(STAND_ALONE_OBJ)
	$(CC) -gstabs -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread  \
		-lm -lsocket -lnsl

FormatVolume: FormatVolume.c
	$(CC) $< -o $@
