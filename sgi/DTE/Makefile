#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1996
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
#  Revision 1.1.2.1  1997/03/18 14:43:10  wenger
#  Various minor changes to get 1.3 to compile on SGI.
#

############################################################

ARCH_FLAGS = -DSGI -I/usr/include/X11 -I/usr/local/include \
	-I/usr/include
ARCH_LDFLAGS =

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.sgi

dte: $(STAND_ALONE_OBJ)
	$(CC) -gstabs -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread -lnsl

FormatVolume: FormatVolume.c
	$(CC) $< -o $@
# DO NOT DELETE
