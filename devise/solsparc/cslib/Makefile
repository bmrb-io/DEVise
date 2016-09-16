#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-2003
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
#  Revision 1.5  2003/01/13 19:26:07  wenger
#  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
#
#  Revision 1.4.22.4  2003/12/19 18:11:56  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.4.22.3.2.2  2003/12/18 19:27:07  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.4.22.3.2.1  2003/12/17 00:18:28  wenger
#  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
#  didn't actually get it to work).
#
#  Revision 1.4.22.3  2003/07/23 22:17:49  wenger
#  DEVise now compiles on Solaris 2.8 (blackbird) instead of 2.7 (cardinal);
#  may still need some mods to get the distribution right.
#
#  Revision 1.4.22.2.2.1  2003/12/16 16:08:49  wenger
#  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
#  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
#  of dynamic library problem).
#
#  Revision 1.4.22.2  2003/01/10 19:49:56  wenger
#  Argh.  Changed all of the Solaris stuff back so that DEVise compiles
#  on Solaris 2.7 (cardinal).
#
#  Revision 1.4.22.1  2003/01/09 16:52:29  wenger
#  Updated linux and solaris dependencies; solsparc now uses Tcl/Tk 8.0.
#
#  Revision 1.4  1998/06/18 21:41:39  wenger
#  Client/server library related fixes and updates.
#
#  Revision 1.3  1998/03/04 18:41:30  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.2  1997/01/18 20:48:46  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.1  1996/10/18 14:04:30  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = $(XLIB_LIBDIR) $(TCL_TK_LIBDIR)
ARCH_SYSLIBS = $(TCL_TK_LIBS) -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include Makefile.depend
