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
#  Revision 1.2  1997/08/12 18:48:44  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.1  1997/04/09 19:10:49  wenger
#  Got unidata to compile -- but not run -- on solaris, solsparc, sun.
#

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DSOLARIS
ARCH_LDFLAGS =
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../unidata
include ../../unidata/Makefile.base
include ../../unidata/Makefile.solaris
