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
#  Revision 1.7.26.1  2003/12/19 18:08:43  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.7.44.1  2003/12/18 19:23:51  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.7  1998/03/11 18:26:08  wenger
#  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
#  reducing include dependencies between csgroup code and the rest of
#  the code, and within the csgroup code.  (Note: running collaboration
#  doesn't work yet.)
#
#  Revision 1.6  1997/11/12 15:48:06  wenger
#  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
#  into the trunk.
#
#  Revision 1.5.6.1  1997/10/30 20:16:45  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.5  1997/05/08 18:11:22  wenger
#  Minor changes to fix Linux compile problems.
#
#  Revision 1.4  1997/01/18 20:49:25  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.3  1996/11/23 20:57:25  jussi
#  Added ARCH_SYSLIBS.
#
#  Revision 1.2  1996/07/13 05:01:37  jussi
#  Updated to compile on Linux.
#
#  Revision 1.1  1996/03/27 18:03:50  wenger
#  Changes to get DEVise to compile and run on Linux.
#
#  Revision 1.3  1996/01/12 18:00:03  jussi
#  Switched to newer compiler version with all necessary header files.
#
#  Revision 1.2  1996/01/12 16:14:17  jussi
#  Gcc header files for HP-UX seem to have vanished; substituted
#  Sun header files instead.
#
#  Revision 1.1  1995/10/15 19:12:14  jussi
#  Initial revision.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = $(TCL_TK_LIBDIR) $(XLIB_LIBDIR)
ARCH_SYSLIBS = $(TCL_TK_LIBS) -ldl -lX11 -lm

VPATH = ../../ams
include $(VPATH)/Makefile.base
include Makefile.depend
