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
#  Revision 1.2.4.1  1997/10/30 20:17:20  wenger
#  Got DEVise to compile, link, and run on sun and linux; compiles but
#  doesn't link on hp (can't find memory mapping functions).  This includes
#  moving the unidata config.h files to separate copies in the unidata
#  directory for each architecture, since the config.h's are architecture-
#  dependent.
#
#  Revision 1.2  1997/08/12 18:47:40  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.1  1997/08/11 14:16:08  wenger
#  Modifications to get DEVise to compile on Linux.
#

############################################################

PERL_CORE = /usr/lib/perl5/i386-linux/5.00307/CORE
#PERL_CORE = /usr/lib/perl5/i486-linux/5.003/CORE

ARCH_FLAGS   = -DLINUX
ARCH_LDFLAGS =
ARCH_LDPOST  =

VPATH = ../../unidata
include ../../unidata/Makefile.base
include ../../unidata/Makefile.linux
