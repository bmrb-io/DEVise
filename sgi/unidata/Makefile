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
#  Revision 1.1.2.1  1997/12/29 22:13:28  wenger
#  Got DEVise 1.4.7 to compile, link, and run on SGI.
#

############################################################

PERL_CORE = /usr/freeware/lib/perl5/irix/CORE

ARCH_FLAGS   = -DSGI
ARCH_LDFLAGS =
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../unidata
include ../../unidata/Makefile.base
include ../../unidata/Makefile.sgi
