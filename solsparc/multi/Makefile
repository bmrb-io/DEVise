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
#  Revision 1.5  1996/10/04 19:59:36  wenger
#  Temporarily (?) removed threads from Devise (removed -lpthread from
#  some Makefiles so Purify works); other minor cleanups.
#
#  Revision 1.4  1996/09/26 18:53:47  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.3  1996/07/08 18:16:00  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.2  1996/05/20 18:38:30  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:12  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib 
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
