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

############################################################

ARCH_FLAGS = -DLINUX -I/usr/include/tcl

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR) $(DTE_DIR)/StandAlone
include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.linux

dte: $(STAND_ALONE_OBJ)
	cd ../devread; make libdevread.a; cd ../DTE;
	$(CC) -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread -lm
