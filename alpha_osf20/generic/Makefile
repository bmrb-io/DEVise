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
#  Revision 1.4  1997/04/11 18:45:52  wenger
#  Added proper static flags to generic Makefiles for various architectures.
#
#  Revision 1.3  1997/01/18 20:49:03  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.2  1996/07/18 14:31:53  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.1  1996/07/14 20:34:40  jussi
#  Initial revision.

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DOSF -I/s/gcc/lib/g++-include -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS = -L/s/X11/lib -L/s/tcl/lib -L/s/gcc/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  =
ARCH_STATIC_LDPOST  = -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.alpha_osf20
