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
#  Revision 1.8  1997/03/20 19:56:48  wenger
#  Minor mods to get latest version to compile on HP and SunOS; cleaned
#  up DTE makefiles (moved all targets from architecture-specific makefiles
#  to DTE/Makefile.base).
#
#  Revision 1.7  1997/03/14 18:37:20  donjerko
#  *** empty log message ***
#
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

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS = -DSUN -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS =
ARCH_GUILDFLAGS = -L/s/X11/lib -L/s/tcl/lib
ARCH_SYSLIBS = -lm
ARCH_GUISYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST =
ARCH_ENDIAN = -DBYTE_BIG_ENDIAN -DWORD_BIG_ENDIAN


DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/ \
	$(DTE_DIR)/Script $(DTE_DIR)/GUI

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.sun
