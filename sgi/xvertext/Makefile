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
#  Revision 1.3  1998/02/26 18:55:35  wenger
#  Got everything to compile on haha -- still have a link problem, though.
#
#  Revision 1.2  1998/01/07 19:31:58  wenger
#  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
#  server library into Devise); updated solaris, sun, linux, and hp
#  dependencies.
#
#  Revision 1.1.2.1  1997/12/30 14:25:21  wenger
#  This somehow didn't get committed before(!).
#
#  Revision 1.1  1997/06/12 17:59:45  wenger

############################################################

IMPLICIT_TEMPLATES = 1
AR = /usr/gnu/bin/ar

ARCH_FLAGS = -DSGI -I/usr/include/X11
ARCH_LDFLAGS = 
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../xvertext
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
