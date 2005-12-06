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
#  Revision 1.17  2003/01/13 19:26:14  wenger
#  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
#
#  Revision 1.16.10.4  2003/12/19 18:12:06  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.16.10.3.2.2  2003/12/18 19:27:17  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.16.10.3.2.1  2003/12/17 00:18:33  wenger
#  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
#  didn't actually get it to work).
#
#  Revision 1.16.10.3  2003/07/23 22:17:56  wenger
#  DEVise now compiles on Solaris 2.8 (blackbird) instead of 2.7 (cardinal);
#  may still need some mods to get the distribution right.
#
#  Revision 1.16.10.2.2.1  2003/12/16 16:08:55  wenger
#  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
#  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
#  of dynamic library problem).
#
#  Revision 1.16.10.2  2003/01/10 19:50:02  wenger
#  Argh.  Changed all of the Solaris stuff back so that DEVise compiles
#  on Solaris 2.7 (cardinal).
#
#  Revision 1.16.10.1  2003/01/09 16:52:34  wenger
#  Updated linux and solaris dependencies; solsparc now uses Tcl/Tk 8.0.
#
#  Revision 1.16  2001/10/17 17:37:13  wenger
#  Linux devised now statically linked to make distributions work.
#
#  Revision 1.15  2001/08/03 18:13:41  wenger
#  Removed all OpenGL-related code.
#
#  Revision 1.14  1999/02/19 15:16:39  wenger
#  Eliminated unnecessary libraries from link (reduces process size).
#
#  Revision 1.13  1999/02/18 18:01:40  wenger
#  OpenGL is now disabled except on SGI platform.
#
#  Revision 1.12  1998/05/26 20:49:08  wenger
#  Changed makefiles for new Tcl/Tk installation.
#
#  Revision 1.11  1998/03/04 18:41:32  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.10  1997/12/23 21:00:35  wenger
#  Got the latest version of DEVise to compile and link on SPARC/Solaris;
#  removed (old) layout manager code from DEVise link.
#
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

include ../Makefile.config

ARCH_LDFLAGS = $(XLIB_LIBDIR) $(TCL_TK_LIBDIR)
ARCH_D_SYSLIBS = -lX11 -lm
ARCH_SYSLIBS = $(TCL_TK_LIBS) $(ARCH_D_SYSLIBS)
ARCH_LDPOST  = -lsocket -lnsl
ARCH_D_LDPOST  = $(ARCH_LDPOST)
ARCH_STATIC_LDPOST  = $(ARCH_LDPOST) -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include Makefile.depend
