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
#  Revision 1.10  1998/03/02 22:32:45  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.9  1998/02/23 23:23:53  wenger
#  Merged cleanup_1_4_7_br_8 thru cleanup_1_4_7_br_9.
#
#  Revision 1.8  1998/01/07 19:31:41  wenger
#  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
#  server library into Devise); updated solaris, sun, linux, and hp
#  dependencies.
#
#  Revision 1.7.4.2  1998/02/23 19:53:19  wenger
#  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.
#
#  Revision 1.7.4.1  1997/12/30 16:50:22  wenger
#  Fixed a few compile problems.
#
#  Revision 1.7  1997/08/12 18:48:17  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.6  1997/03/25 18:01:43  wenger
#  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.
#
#  Revision 1.5.4.1  1997/03/18 14:43:37  wenger
#  Various minor changes to get 1.3 to compile on SGI.
#
#  Revision 1.5  1997/01/18 20:49:22  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/07/08 18:15:32  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.3  1995/12/29 18:54:40  jussi
#  Removed -fno-for-scope compiler flag.
#
#  Revision 1.2  1995/12/28 20:53:37  jussi
#  Updated ARCH_FLAGS to reflect SGI platform.
#
#  Revision 1.1  1995/12/28 16:12:00  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L /usr/lib/X11 -L /usr/freeware/lib 
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
