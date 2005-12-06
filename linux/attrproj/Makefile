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
#  Revision 1.6.26.1  2003/12/19 18:08:48  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.6.44.1  2003/12/18 19:23:57  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.6  1998/03/11 18:26:14  wenger
#  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
#  reducing include dependencies between csgroup code and the rest of
#  the code, and within the csgroup code.  (Note: running collaboration
#  doesn't work yet.)
#
#  Revision 1.5  1997/11/12 15:48:13  wenger
#  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
#  into the trunk.
#
#  Revision 1.4.6.1  1997/10/30 20:16:52  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.4  1997/05/08 18:11:24  wenger
#  Minor changes to fix Linux compile problems.
#
#  Revision 1.3  1996/09/25 18:37:08  wenger
#  Updated again for Linux compile (Linux 2.0.6, Tcl 7.4, Tk 4.0).
#
#  Revision 1.2  1996/07/13 05:01:38  jussi
#  Updated to compile on Linux.
#
#  Revision 1.1  1996/04/22 18:06:14  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = $(TCL_TK_LIBDIR)
ARCH_SYSLIBS = $(TCL_TK_LIBS) -ldl -lm
ARCH_LDPOST  = -static

VPATH = ../../attrproj:../../graphics:../../graphics.new
include ../../attrproj/Makefile.base
include Makefile.depend
