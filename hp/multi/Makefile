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
#  Revision 1.5  1997/01/18 20:48:55  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/07/18 13:58:49  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.3  1996/07/13 02:43:47  jussi
#  Added -static linker option.
#
#  Revision 1.2  1996/07/08 18:14:44  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.1  1995/10/15 19:12:19  jussi
#  Initial revision.

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  =
#ARCH_LDPOST  = -lSM -lICE -static

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.hp
