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
#  Revision 1.4  1996/11/23 23:01:37  jussi
#  Fixed to operate properly on HP.
#
#  Revision 1.3  1996/11/23 01:52:12  donjerko
#  added standalone option
#
#  Revision 1.2  1996/11/21 16:22:51  wenger
#  Updated dependencies; fixed some compile errors and warnings.
#
#  Revision 1.1  1996/11/19 15:33:36  wenger
#  Added HP DTE makefile.
#

############################################################

ARCH_FLAGS = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.hp

dte: $(STAND_ALONE_OBJ)
	$(CC) -gstabs -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread -lm -lsocket -lnsl

FormatVolume: FormatVolume.c
	$(CC) $< -o $@
