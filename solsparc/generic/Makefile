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
#  Revision 1.4  1996/08/09 15:40:46  wenger
#  Modified makefiles to allow making static and Purify'd executables.
#
#  Revision 1.3  1996/05/31 21:36:22  wenger
#  Fixed core dump in SPARC/Solaris version caused by GData buffer
#  misalignment; cleaned up generic/Makefile.base, etc., to get HP
#  version to link correctly and eliminate special Makefile.base.aix.
#
#  Revision 1.2  1996/05/20 18:38:28  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:09  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lXmu -lXt -lXext -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl -lpthread
ARCH_STATIC_LDPOST  = -lsocket -lnsl -lpthread -static
# Note: Purify works on SPARC/Solaris without the collector being specified.
#ARCH_COLLECTOR	= /usr/ccs/bin/ld

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
