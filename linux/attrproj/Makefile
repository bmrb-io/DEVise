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
#  Revision 1.2  1996/07/13 05:01:38  jussi
#  Updated to compile on Linux.
#
#  Revision 1.1  1996/04/22 18:06:14  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.

############################################################

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib
ARCH_SYSLIBS = -ltcl7.4 -lm
ARCH_LDPOST  = -static

VPATH = ../../attrproj:../../graphics:../../graphics.new
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.linux
