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
#  Revision 1.5  1998/02/20 17:03:26  wenger
#  Updated all architectures to use new dependency scheme in graphics and
#  graphics.new directories.
#
#  Revision 1.4  1997/09/23 20:02:01  wenger
#  Opening and saving of sessions now working with new scheme of mapping
#  automatically creating the appropriate TData.
#
#  Revision 1.3  1997/04/03 21:31:48  wenger
#  Eliminated unnecessary tcl dependencies in graphics directory.
#
#  Revision 1.2  1996/05/20 18:38:29  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:10  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS =
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../graphics
include $(VPATH)/Makefile.base
include Makefile.depend
