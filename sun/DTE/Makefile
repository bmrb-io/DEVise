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
#  Revision 1.1  1996/11/19 15:33:52  wenger
#  Added Sun DTE makefile.
#

############################################################

ARCH_FLAGS = -DSUN 

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR) $(DTE_DIR)/StandAlone
include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.solaris

dte: $(STAND_ALONE_OBJ)
        cd ../devread; make libdevread.a; cd ../DTE;
        $(CC) -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread -lm -lsocket -lnsl

