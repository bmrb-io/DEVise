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
#  Revision 1.3  1998/03/02 22:31:19  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.2  1997/03/25 18:01:16  wenger
#  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.
#
#  Revision 1.1.2.1  1997/03/18 14:43:20  wenger
#  Various minor changes to get 1.3 to compile on SGI.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_LDPOST  = 

VPATH = ../../attrproj:../../graphics:../../graphics.new:../../DTE/RTree
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.sgi
