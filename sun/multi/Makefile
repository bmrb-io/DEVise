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
#  Revision 1.9  1996/07/13 02:18:15  jussi
#  Removed /usr/unsup/lib which appeared unnecessary.
#
#  Revision 1.8  1996/07/08 18:16:13  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.7  1995/09/28 15:10:05  jussi
#  Added copyright and CVS log messages.
#

############################################################

ARCH_FLAGS   = -DSUN -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  = -static

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sun
