#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1997
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
#  Revision 1.2  1998/01/07 19:31:26  wenger
#  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
#  server library into Devise); updated solaris, sun, linux, and hp
#  dependencies.
#
#  Revision 1.1.2.1  1997/12/29 22:13:14  wenger
#  Got DEVise 1.4.7 to compile, link, and run on SGI.
#

############################################################

IMPLICIT_TEMPLATES = 1
AR = /usr/gnu/bin/ar

ARCH_FLAGS = -DSGI -I/s/X11/include -I/usr/local/include

VPATH = ../../LM
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
