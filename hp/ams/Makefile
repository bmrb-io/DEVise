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
#  Revision 1.3  1996/01/12 18:00:03  jussi
#  Switched to newer compiler version with all necessary header files.
#
#  Revision 1.2  1996/01/12 16:14:17  jussi
#  Gcc header files for HP-UX seem to have vanished; substituted
#  Sun header files instead.
#
#  Revision 1.1  1995/10/15 19:12:14  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lXaw -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../ams
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.hp
