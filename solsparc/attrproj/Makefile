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
#  Revision 1.3  1996/06/28 13:57:29  wenger
#  Forgot to change the attrproj Makefiles for all architectures.
#
#  Revision 1.2  1996/05/20 18:38:26  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:08  jussi
#  Initial revision.

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/tcl/include
ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_LDPOST  =

VPATH = ../../attrproj:../../graphics:../../graphics.new
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.solaris
