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
#  Revision 1.8.4.1  1997/12/29 22:13:21  wenger
#  Got DEVise 1.4.7 to compile, link, and run on SGI.
#
#  Revision 1.8  1997/08/12 18:48:08  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.7  1997/04/11 18:46:17  wenger
#  Added proper static flags to generic Makefiles for various architectures.
#
#  Revision 1.6  1997/01/18 20:49:21  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.5  1996/07/15 14:20:30  jussi
#  Updated to reflect new Tcl/Tk library.
#
#  Revision 1.4  1996/05/31 21:36:05  wenger
#  Fixed core dump in SPARC/Solaris version caused by GData buffer
#  misalignment; cleaned up generic/Makefile.base, etc., to get HP
#  version to link correctly and eliminate special Makefile.base.aix.
#
#  Revision 1.3  1995/12/29 18:54:38  jussi
#  Removed -fno-for-scope compiler flag.
#
#  Revision 1.2  1995/12/28 20:53:35  jussi
#  Updated ARCH_FLAGS to reflect SGI platform.
#
#  Revision 1.1  1995/12/28 16:11:57  jussi
#  Initial revision.
#

############################################################

PERL_CORE = /usr/freeware/lib/perl5/irix/CORE

ARCH_FLAGS   = -DSGI -I/usr/include/X11 -I/usr/local/include

# Having -L /usr/local/lib here makes strstreams core dump! RKW Dec. 29, 1997.
ARCH_LDFLAGS = -L /usr/lib/X11

ARCH_SYSLIBS = -ltk4.1 -ltcl7.5 -lX11 -lm
ARCH_LDPOST  =
ARCH_STATIC_LDPOST  = -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
