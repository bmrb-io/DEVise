#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1996
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
#  Revision 1.2  1998/02/20 17:00:30  wenger
#  Updated all architectures to use new dependency scheme in graphics and
#  graphics.new directories.
#
#  Revision 1.1  1996/07/14 20:34:40  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/tcl/lib -L/s/gcc/lib
ARCH_LDPOST  =

VPATH = ../../graphics
include $(VPATH)/Makefile.base
include Makefile.depend
