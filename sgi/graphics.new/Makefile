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
#  Revision 1.5  1998/02/20 17:03:11  wenger
#  Updated all architectures to use new dependency scheme in graphics and
#  graphics.new directories.
#
#  Revision 1.4.16.1  1998/02/23 19:53:04  wenger
#  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.
#
#  Revision 1.4  1995/12/29 18:54:39  jussi
#  Removed -fno-for-scope compiler flag.
#
#  Revision 1.3  1995/12/28 21:53:37  jussi
#  Added -I/usr/include compiler flag.
#
#  Revision 1.2  1995/12/28 20:53:36  jussi
#  Updated ARCH_FLAGS to reflect SGI platform.
#
#  Revision 1.1  1995/12/28 16:11:59  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS = -DSGI -I/usr/include/X11 -I/usr/local/include \
	-I/usr/include -I/usr/freeware/include

VPATH = ../../graphics.new
include $(VPATH)/Makefile.base
include Makefile.depend
