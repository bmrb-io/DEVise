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
#  Revision 1.5  1998/03/04 18:41:07  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.4  1998/02/26 17:20:40  wenger
#  Fixed problems with yesterday's commit.
#
#  Revision 1.2  1997/01/18 18:58:29  jussi
#  Removed inclusion of Xaw and related libraries.
#
#  Revision 1.1  1996/10/17 20:50:38  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl7.4/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
