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
#  Revision 1.6  1997/01/10 17:05:37  wenger
#  Fixed DTE makefiles for HP and Sun.
#
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
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/ \
	$(DTE_DIR)/Script $(DTE_DIR)/GUI

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.sun

dte: $(STAND_ALONE_OBJ)
	$(CC) -gstabs -o dte $(STAND_ALONE_OBJ) -L../devread -ldevread \
		-lm

script: $(SCRIPT_OBJ)
	$(CC) -o script $(SCRIPT_OBJ) -L../devread -ldevread -lm

gui: $(GUI_OBJ)
	$(CC) -gstabs -o gui $(GUI_OBJ) -L../devread -L/s/X11/lib -L/s/tcl/lib \
		-ldevread -lm -ltk -ltcl -lX11

FormatVolume: FormatVolume.c
	$(CC) $< -o $@

convert_bulk: convert_bulk.C correct_math.C heap.o
	$(CC) heap.o $< -o $@

