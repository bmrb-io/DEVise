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
#  Revision 1.12  1998/03/04 18:40:45  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.11  1998/03/02 22:32:10  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.10  1998/02/23 23:23:28  wenger
#  Merged cleanup_1_4_7_br_8 thru cleanup_1_4_7_br_9.
#
#  Revision 1.9  1998/01/07 19:31:34  wenger
#  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
#  server library into Devise); updated solaris, sun, linux, and hp
#  dependencies.
#
#  Revision 1.8.4.2  1998/02/23 19:52:51  wenger
#  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.
#
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

include ../Makefile.config

# Having -L /usr/local/lib here makes strstreams core dump! RKW Dec. 29, 1997.
ARCH_LDFLAGS = -L /usr/lib/X11 -L /usr/freeware/lib

# Putting -lclient in here is a kludge, because for some ****ed-up reason
# we need to link it on the SGI even though we don't need it on the
# other architectures.
ARCH_SYSLIBS = -lclient -lGL -lGLU -ltk4.0 -ltcl7.4 -lX11 -lm

ARCH_LDPOST  =
ARCH_STATIC_LDPOST  = -static

VPATH = ../../generic

ifeq ($(DEV_DEBUG_GENERIC),1)
  DEV_DEBUG = 1
endif

include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
