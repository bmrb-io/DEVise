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
#  Revision 1.4  1997/11/12 15:48:22  wenger
#  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
#  into the trunk.
#
#  Revision 1.3.6.1  1997/10/30 20:16:59  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.3  1997/05/08 18:11:26  wenger
#  Minor changes to fix Linux compile problems.
#
#  Revision 1.2  1997/01/18 20:49:25  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.1  1996/10/17 20:50:36  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/usr/lib -L/usr/X11R6/lib
#ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
#ARCH_SYSLIBS = -ltk4.1 -ltcl7.5 -ldl -lX11 -lm
ARCH_SYSLIBS = -ltk4.2 -ltcl7.6 -ldl -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../cslib
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.linux
