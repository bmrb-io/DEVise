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
#  Revision 1.15  1997/01/18 18:58:30  jussi
#  Removed inclusion of Xaw and related libraries.
#
#  Revision 1.14  1996/11/23 20:54:15  jussi
#  Removed -lpthread.
#
#  Revision 1.13  1996/10/04 19:59:10  wenger
#  Temporarily (?) removed threads from Devise (removed -lpthread from
#  some Makefiles so Purify works); other minor cleanups.
#
#  Revision 1.12  1996/09/26 18:54:00  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.11  1996/08/09 15:40:35  wenger
#  Modified makefiles to allow making static and Purify'd executables.
#
#  Revision 1.10  1996/07/23 19:28:51  beyer
#  Added Xaw to the list of libraries (needed for gcc 2.6.3)
#
#  Revision 1.9  1996/05/31 21:36:13  wenger
#  Fixed core dump in SPARC/Solaris version caused by GData buffer
#  misalignment; cleaned up generic/Makefile.base, etc., to get HP
#  version to link correctly and eliminate special Makefile.base.aix.
#
#  Revision 1.8  1996/05/20 18:37:57  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.7  1996/02/13 16:44:17  jussi
#  Replaced /usr/X11R6/include with /s/X11/include.
#
#  Revision 1.6  1996/01/09 16:44:08  jussi
#  Added -lnsl for network support.
#
#  Revision 1.5  1995/09/28 15:12:00  jussi
#  Added copyright and CVS log messages.
#

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl
ARCH_STATIC_LDPOST  = -lsocket -lnsl -static
# Note: Purify is not available on Pentium/Solaris.
#ARCH_COLLECTOR	= /usr/ccs/bin/ld

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
