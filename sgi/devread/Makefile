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
#  Revision 1.3  1997/03/25 18:01:32  wenger
#  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.
#
#  Revision 1.2.4.1  1997/03/18 14:43:28  wenger
#  Various minor changes to get 1.3 to compile on SGI.
#

############################################################

IMPLICIT_TEMPLATES = 1
AR = /usr/gnu/bin/ar

ARCH_FLAGS   = -DSGI -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_LDPOST  =

VPATH = ../../devread:../../graphics:../../graphics.new
include ../../devread/Makefile.base
include ../../devread/Makefile.sgi
