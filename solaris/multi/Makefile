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
#  Revision 1.13  1997/01/18 20:48:38  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.12  1996/10/04 19:59:17  wenger
#  Temporarily (?) removed threads from Devise (removed -lpthread from
#  some Makefiles so Purify works); other minor cleanups.
#
#  Revision 1.11  1996/09/26 18:54:02  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.10  1996/07/08 18:15:47  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.9  1996/05/20 18:38:01  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.8  1996/02/13 16:44:21  jussi
#  Replaced /usr/X11R6/include with /s/X11/include.
#
#  Revision 1.7  1996/01/09 16:47:46  jussi
#  Added -lnsl for network support.
#
#  Revision 1.6  1995/09/28 15:12:04  jussi
#  Added copyright and CVS log messages.
#

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib 
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
