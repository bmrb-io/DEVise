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
#  Revision 1.21  1998/03/04 18:41:09  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.20  1998/02/26 17:20:52  wenger
#  Fixed problems with yesterday's commit.
#
#  Revision 1.18  1997/12/16 18:02:45  zhenhai
#  Added OpenGL features.
#
#  Revision 1.17  1997/12/04 18:31:57  wenger
#  Merged new expression evaluation code thru the expression_br_2 tag.
#
#  Revision 1.16.6.2  1997/12/04 15:50:05  wenger
#  Undid Haihong's temporary changes.
#
#  Revision 1.16.6.1  1997/12/03 22:52:55  whh
#  Devise now uses new C++ expression evaluation code instead of Tcl
#  interpreter.
#
#  Revision 1.16  1997/08/12 18:48:32  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
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

include ../Makefile.config

ARCH_LDFLAGS = -L/s/mesa-2.4/lib -R/s/mesa-2.4/lib \
               -L/s/X11/lib -L /s/tcl/lib -R /s/X11/lib
ARCH_SYSLIBS = -lMesaGLw -lMesaGLU -lMesaGL \
               -ltk -ltcl -lXaw -lXpm -lXt -lXmu -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl
ARCH_STATIC_LDPOST  = -lsocket -lnsl -static

VPATH = ../../generic

ifeq ($(DEV_DEBUG_GENERIC),1)
  DEV_DEBUG = 1
endif

include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.solaris
