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
#  Revision 1.8  1997/08/12 18:46:56  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.7  1997/01/18 20:48:54  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.6  1996/08/09 15:40:13  wenger
#  Modified makefiles to allow making static and Purify'd executables.
#
#  Revision 1.5  1996/07/18 13:58:48  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.4  1996/05/31 21:35:46  wenger
#  Fixed core dump in SPARC/Solaris version caused by GData buffer
#  misalignment; cleaned up generic/Makefile.base, etc., to get HP
#  version to link correctly and eliminate special Makefile.base.aix.
#
#  Revision 1.3  1996/02/26 16:44:32  jussi
#  Makes a statically linked executable.
#
#  Revision 1.2  1996/01/17 22:37:29  jussi
#  Removed -static from ARCH_LDPOST.
#
#  Revision 1.1  1995/10/15 19:12:16  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  =
ARCH_STATIC_LDPOST  = -lSM -lICE -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.hp
