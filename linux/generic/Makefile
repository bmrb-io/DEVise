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
#  Revision 1.8.4.1  1997/10/30 20:17:06  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.8  1997/08/12 18:47:25  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.7  1997/05/08 18:11:27  wenger
#  Minor changes to fix Linux compile problems.
#
#  Revision 1.6  1997/04/11 18:46:02  wenger
#  Added proper static flags to generic Makefiles for various architectures.
#
#  Revision 1.5  1997/01/18 20:49:26  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/09/25 18:37:15  wenger
#  Updated again for Linux compile (Linux 2.0.6, Tcl 7.4, Tk 4.0).
#
#  Revision 1.3  1996/07/13 05:01:38  jussi
#  Updated to compile on Linux.
#
#  Revision 1.2  1996/05/31 21:35:56  wenger
#  Fixed core dump in SPARC/Solaris version caused by GData buffer
#  misalignment; cleaned up generic/Makefile.base, etc., to get HP
#  version to link correctly and eliminate special Makefile.base.aix.
#
#  Revision 1.1  1996/03/27 18:04:01  wenger
#  Changes to get DEVise to compile and run on Linux.
#
#  Revision 1.3  1996/02/26 16:44:32  jussi
#  Makes a statically linked executable.
#
#  Revision 1.2  1996/01/17 22:37:29  jussi
#  Removed -static from ARCH_LDPOST.
#
#  Revision 1.1  1995/10/15 19:12:16  jussi
#  Initial revision.

############################################################

PERL_CORE = /usr/lib/perl5/i386-linux/5.00307/CORE
#PERL_CORE = /usr/lib/perl5/i486-linux/5.003/CORE

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib -L/usr/X11R6/lib
#ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
#ARCH_SYSLIBS = -ltk4.1 -ltcl7.5 -ldl -lX11 -lm
ARCH_SYSLIBS = -ltk4.2 -ltcl7.6 -ldl -lX11 -lm
ARCH_LDPOST  =
ARCH_STATIC_LDPOST  = -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
