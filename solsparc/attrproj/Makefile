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
#  Revision 1.7  1997/02/03 04:14:33  donjerko
#  Catalog management moved to DTE
#
#  Revision 1.6  1996/11/23 20:56:08  jussi
#  Removed -lpthread.
#
#  Revision 1.5  1996/09/26 18:53:45  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.4  1996/07/18 14:17:24  jussi
#  Removed -static flag to save disk space.
#
#  Revision 1.3  1996/06/28 13:57:29  wenger
#  Forgot to change the attrproj Makefiles for all architectures.
#
#  Revision 1.2  1996/05/20 18:38:26  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:08  jussi
#  Initial revision.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_LDPOST  =

VPATH = ../../attrproj:../../graphics:../../graphics.new:../../DTE/RTree
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.solaris
