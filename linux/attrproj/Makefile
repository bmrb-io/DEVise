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
#  Revision 1.4.6.1  1997/10/30 20:16:52  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.4  1997/05/08 18:11:24  wenger
#  Minor changes to fix Linux compile problems.
#
#  Revision 1.3  1996/09/25 18:37:08  wenger
#  Updated again for Linux compile (Linux 2.0.6, Tcl 7.4, Tk 4.0).
#
#  Revision 1.2  1996/07/13 05:01:38  jussi
#  Updated to compile on Linux.
#
#  Revision 1.1  1996/04/22 18:06:14  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.

############################################################

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib
#ARCH_SYSLIBS = -ltcl7.4 -lm
#ARCH_SYSLIBS = -ltcl7.5 -ldl -lm
ARCH_SYSLIBS = -ltcl7.6 -ldl -lm
ARCH_LDPOST  = -static

VPATH = ../../attrproj:../../graphics:../../graphics.new
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.linux
