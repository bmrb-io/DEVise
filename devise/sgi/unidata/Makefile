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
#  Revision 1.4  1998/03/02 22:33:07  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.3  1998/02/23 23:23:59  wenger
#  Merged cleanup_1_4_7_br_8 thru cleanup_1_4_7_br_9.
#
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

include ../Makefile.config

ARCH_LDFLAGS =
ARCH_LDPOST  = -lnsl

VPATH = ../../unidata
include ../../unidata/Makefile.base
include ../../unidata/Makefile.sgi
