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
#  Revision 1.1.2.1  1997/03/18 14:43:20  wenger
#  Various minor changes to get 1.3 to compile on SGI.
#

############################################################

ARCH_FLAGS = -DSGI -I/usr/include/X11 -I/usr/local/include \
	-I/usr/include
ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_LDPOST  = 

VPATH = ../../attrproj:../../graphics:../../graphics.new:../../DTE/RTree
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.sgi
