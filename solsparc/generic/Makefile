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
#  Revision 1.9  1997/08/12 18:48:57  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.8  1997/01/18 20:48:47  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.7  1996/11/23 20:56:14  jussi
#  Removed -lpthread.
#
#  Revision 1.6  1996/10/04 19:59:25  wenger
#  Temporarily (?) removed threads from Devise (removed -lpthread from
#  some Makefiles so Purify works); other minor cleanups.
#
#  Revision 1.5  1996/09/26 18:53:46  jussi
#  Added -lpthread to get thread support.
#
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

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DSOLARIS -I/s/X11/include -I/s/tcl/include

# We need to explicitly specify /usr/lib here so we get the libm.a there
# rather than from /s/mesa-2.4/lib.
ARCH_LDFLAGS = -L/usr/lib -L/s/mesa-2.4/lib -R/s/mesa-2.4/lib \
		-L /s/X11/lib -L /s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -lMesaGLw -lMesaGLU -lMesaGL \
		-ltk -ltcl -lXaw -lXpm -lXt -lXmu -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl
ARCH_STATIC_LDPOST  = -lsocket -lnsl -static
# Note: Purify works on SPARC/Solaris without the collector being specified.
#ARCH_COLLECTOR	= /usr/ccs/bin/ld

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
