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

ARCH_FLAGS   = -DLINUX -I/usr/include/tcl
ARCH_LDFLAGS = -L/usr/lib -L/usr/X11/lib
#ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
#ARCH_SYSLIBS = -ltk4.1 -ltcl7.5 -ldl -lX11 -lm
ARCH_SYSLIBS = -ltk4.2 -ltcl7.6 -ldl -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../ams
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
