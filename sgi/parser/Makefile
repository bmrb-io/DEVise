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
#  Revision 1.3  1995/12/29 18:54:42  jussi
#  Removed -fno-for-scope compiler flag.
#
#  Revision 1.2  1995/12/28 20:53:38  jussi
#  Updated ARCH_FLAGS to reflect SGI platform.
#
#  Revision 1.1  1995/12/28 16:12:01  jussi
#  Initial revision.
#

############################################################

IMPLICIT_TEMPLATES = 1
AR = /usr/gnu/bin/ar

ARCH_FLAGS = -DSGI -I../../parser

VPATH = ../../parser
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
