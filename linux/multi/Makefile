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
#  Revision 1.9.26.1  2003/12/19 18:09:32  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.9.44.1  2003/12/18 19:24:44  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.9  1998/03/11 18:27:21  wenger
#  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
#  reducing include dependencies between csgroup code and the rest of
#  the code, and within the csgroup code.  (Note: running collaboration
#  doesn't work yet.)
#
#  Revision 1.8  1997/11/12 15:48:39  wenger
#  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
#  into the trunk.
#
#  Revision 1.7.4.1  1997/10/30 20:17:12  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.7  1997/08/12 18:47:32  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.6  1997/05/08 18:11:29  wenger
#  Minor changes to fix Linux compile problems.
#
#  Revision 1.5  1997/01/18 20:49:26  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/09/25 18:37:23  wenger
#  Updated again for Linux compile (Linux 2.0.6, Tcl 7.4, Tk 4.0).
#
#  Revision 1.3  1996/07/13 05:01:39  jussi
#  Updated to compile on Linux.
#
#  Revision 1.2  1996/07/08 18:15:00  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.1  1996/03/27 18:04:14  wenger
#  Changes to get DEVise to compile and run on Linux.
#
#  Revision 1.1  1995/10/15 19:12:19  jussi
#  Initial revision.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = $(TCL_TK_LIBDIR) $(XLIB_LIBDIR)
ARCH_SYSLIBS = $(TCL_TK_LIBS) -ldl -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../multi
include $(VPATH)/Makefile.base
include Makefile.depend
