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
#  Revision 1.2  1996/05/20 18:37:56  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/04/22 18:05:34  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.
#
#

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/tcl/include
ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_LDPOST  = -static

VPATH = ../../attrproj:../../graphics
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.solaris
