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
#  Revision 1.14  1998/03/04 18:41:56  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.13  1997/08/12 18:49:21  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.12  1997/01/18 20:49:16  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.11  1996/08/09 15:40:57  wenger
#  Modified makefiles to allow making static and Purify'd executables.
#
#  Revision 1.10  1996/07/18 14:10:25  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.9  1996/07/13 02:17:57  jussi
#  Removed /usr/unsup/lib which appeared unnecessary.
#
#  Revision 1.8  1996/05/31 21:36:31  wenger
#  Fixed core dump in SPARC/Solaris version caused by GData buffer
#  misalignment; cleaned up generic/Makefile.base, etc., to get HP
#  version to link correctly and eliminate special Makefile.base.aix.
#
#  Revision 1.7  1996/03/05 23:53:00  jussi
#  Executable now statically linked.
#
#  Revision 1.6  1995/09/28 15:10:00  jussi
#  Added copyright and CVS log messages.

############################################################

include ../Makefile.config

ARCH_LDFLAGS =  -L/s/tcl7.4/lib
ARCH_SYSLIBS = -lopengldum -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =
ARCH_STATIC_LDPOST  = -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sun
