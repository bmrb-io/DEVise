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
#  Revision 1.9  1997/03/20 19:56:20  wenger
#  Minor mods to get latest version to compile on HP and SunOS; cleaned
#  up DTE makefiles (moved all targets from architecture-specific makefiles
#  to DTE/Makefile.base).
#
#  Revision 1.8  1997/03/11 22:50:16  donjerko
#  *** empty log message ***
#
#  Revision 1.7  1997/02/03 04:12:27  donjerko
#  Catalog management moved to DTE
#
#  Revision 1.6  1997/01/10 17:05:26  wenger
#  Fixed DTE makefiles for HP and Sun.
#  : ----------------------------------------------------------------------
#
#  Revision 1.5  1996/12/27 17:00:21  wenger
#  Got latest code to compile for sun, solsparc, and hp; updated dependencies;
#  changed 'Import' to 'New Table' in main window GUI to reflect change of
#  function.
#
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

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS =
ARCH_GUILDFLAGS = -L/s/X11/lib -L/s/tcl/lib
ARCH_SYSLIBS = -lm
ARCH_GUISYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST =
ARCH_ENDIAN =

DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools $(DTE_DIR)/Script \
	../../attrproj/ $(DTE_DIR)/GUI

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.hp
