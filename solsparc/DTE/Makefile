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
#  Revision 1.3  1996/11/26 21:42:47  wenger
#  Fixed various makefile problems; updated dependencies.
#
#  Revision 1.2  1996/11/23 01:54:37  donjerko
#  added standalone option
#
#  Revision 1.1  1996/11/19 15:23:50  donjerko
#  *** empty log message ***
#
############################################################

ARCH_FLAGS = -DSOLARIS -I/s/X11/include -I/s/tcl/include 
#ARCH_LDFLAGS = -lpthread

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/
include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.solaris

dte: $(STAND_ALONE_OBJ)
	$(CC) -gstabs -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread \
		-lm -lsocket -lnsl

FormatVolume: FormatVolume.c
	$(CC) $< -o $@
