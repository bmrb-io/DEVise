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
#  Revision 1.6  1997/03/25 18:01:43  wenger
#  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.
#
#  Revision 1.5.4.1  1997/03/18 14:43:37  wenger
#  Various minor changes to get 1.3 to compile on SGI.
#
#  Revision 1.5  1997/01/18 20:49:22  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/07/08 18:15:32  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.3  1995/12/29 18:54:40  jussi
#  Removed -fno-for-scope compiler flag.
#
#  Revision 1.2  1995/12/28 20:53:37  jussi
#  Updated ARCH_FLAGS to reflect SGI platform.
#
#  Revision 1.1  1995/12/28 16:12:00  jussi
#  Initial revision.
#

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DSGI -I/usr/include/X11 -I/usr/local/include
ARCH_LDFLAGS = -L /usr/lib/X11 -L /usr/local/lib 
ARCH_SYSLIBS = -ltk4.1 -ltcl7.5 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sgi
