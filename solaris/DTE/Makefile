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
#  Revision 1.7  1996/12/24 21:01:10  kmurli
#  Included FunctionRead to support joinprev and joinnext
#
#  Revision 1.6  1996/12/16 07:22:15  donjerko
#  Fixed some compile problems
#
#  Revision 1.5  1996/12/15 06:29:43  donjerko
#  Added two binary files to support RTrees (temporary fix)
#
#  Revision 1.4  1996/11/25 04:22:02  donjerko
#  Removed "make libdevread.a"
#
#  Revision 1.3  1996/11/23 20:54:53  jussi
#  Removed -lpthread.
#
#  Revision 1.2  1996/11/23 01:53:29  donjerko
#  added standalone option
#
#  Revision 1.1  1996/11/18 21:08:05  donjerko
#  *** empty log message ***
#
############################################################

ARCH_FLAGS = -DSOLARIS -I/s/X11/include -I/s/tcl/include 
ARCH_LDFLAGS =

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools $(DTE_DIR)/Script ../../attrproj/

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.solaris

dte: $(STAND_ALONE_OBJ)
	$(CC) -gstabs -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread -lm -lsocket -lnsl

script: $(SCRIPT_OBJ)
	$(CC) -o script $(SCRIPT_OBJ) -L../devread -ldevread -lm -lsocket -lnsl

FormatVolume: FormatVolume.c
	$(CC) $< -o $@
# DO NOT DELETE
