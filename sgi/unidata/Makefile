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
#  Revision 1.2  1998/01/07 19:31:49  wenger
#  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
#  server library into Devise); updated solaris, sun, linux, and hp
#  dependencies.
#
#  Revision 1.1.2.2  1998/02/23 19:53:25  wenger
#  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.
#
#  Revision 1.1.2.1  1997/12/29 22:13:28  wenger
#  Got DEVise 1.4.7 to compile, link, and run on SGI.
#

############################################################

PERL_CORE = /usr/freeware/lib/perl5/irix/CORE

ARCH_FLAGS   = -DSGI
ARCH_LDFLAGS =
ARCH_LDPOST  = -lnsl

VPATH = ../../unidata
include ../../unidata/Makefile.base
include ../../unidata/Makefile.sgi
