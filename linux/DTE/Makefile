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
#  Revision 1.7  1997/11/12 15:47:57  wenger
#  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
#  into the trunk.
#
#  Revision 1.6.4.1  1997/10/30 20:16:37  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.6  1997/08/12 18:47:17  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.5  1997/05/08 18:11:19  wenger
#  Minor changes to fix Linux compile problems.
#
#  Revision 1.4  1997/03/20 19:56:26  wenger
#  Minor mods to get latest version to compile on HP and SunOS; cleaned
#  up DTE makefiles (moved all targets from architecture-specific makefiles
#  to DTE/Makefile.base).
#
#  Revision 1.3  1997/01/21 19:01:54  wenger
#  Minor changes to get Linux compile to work, updated Linux dependencies.
#
#  Revision 1.2  1996/12/30 17:51:23  wenger
#  Got latest sources to compile on Linux; updated Linux dependencies;
#  changes devise.dali to devise.tasvir in release script.
#
#  Revision 1.1  1996/12/06 19:00:36  wenger
#  Fixed up linux dirs so latest version compiles for Linux; updated
#  Linux dependencies.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS =
ARCH_GUILDFLAGS = -L/usr/X11R6/lib -L/usr/lib
ARCH_SYSLIBS = -lm
#ARCH_GUISYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
#ARCH_GUISYSLIBS = -ltk4.1 -ltcl7.5 -ldl -lX11 -lm
ARCH_GUISYSLIBS = -ltk4.2 -ltcl7.6 -ldl -lX11 -lm
ARCH_LDPOST =


DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
        $(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools $(DTE_DIR)/Script \
        ../../attrproj/ $(DTE_DIR)/GUI

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.linux
