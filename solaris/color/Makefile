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
#  Revision 1.2  1997/12/04 23:51:15  wenger
#  Changed how dependencies work for src/color and src/utils to work
#  around a bug in makedepend (this is an improvement anyhow).
#
#  Revision 1.1  1997/12/02 17:35:44  wenger
#  Set up src/color and src/utils to compile "normally" for solaris and
#  solsparc.
#

############################################################

ARCH_FLAGS = -DSGI -I/s/X11/include -I/usr/freeware/src/perl5.002
ARCH_LDFLAGS = 
ARCH_LDPOST  =

VPATH = ../../src/color
include $(VPATH)/Makefile.base
include Makefile.depend
