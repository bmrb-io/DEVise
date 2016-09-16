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
#  Revision 1.2  1996/07/18 14:22:54  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.1  1996/07/14 20:34:37  jussi
#  Initial revision.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib -L/s/gcc/lib
ARCH_LDPOST  =

VPATH = ../../attrproj:../../graphics:../../graphics.new
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.alpha_osf20
