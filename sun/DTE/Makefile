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
#  Revision 1.5  1996/12/27 17:01:06  wenger
#  Got latest code to compile for sun, solsparc, and hp; updated dependencies;
#  changed 'Import' to 'New Table' in main window GUI to reflect change of
#  function.
#
#  Revision 1.4  1996/11/23 23:01:48  jussi
#  Fixed to operate properly on HP.
#
#  Revision 1.3  1996/11/23 22:47:37  jussi
#  Added tcl include directory to ARCH_FLAGS.
#
#  Revision 1.2  1996/11/23 01:58:52  donjerko
#  added standalone option
#
#  Revision 1.1  1996/11/19 15:33:52  wenger
#  Added Sun DTE makefile.
#

############################################################

ARCH_FLAGS = -DSUN -I/s/X11/include -I/s/tcl/include

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/
include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.sun

dte: $(STAND_ALONE_OBJ)
	$(CC) -gstabs -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread \
		-lm

FormatVolume: FormatVolume.c
	$(CC) $< -o $@
