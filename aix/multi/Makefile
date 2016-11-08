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
#  Revision 1.6  1997/01/18 20:49:11  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.5  1996/07/13 02:12:07  jussi
#  AIX now also uses the base Makefile.
#
#  Revision 1.4  1996/07/08 18:14:25  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.3  1996/05/31 15:45:32  jussi
#  A customized Makefile.base.aix is used in AIX.
#
#  Revision 1.2  1996/05/22 21:00:38  jussi
#  Merged Makefile.base.aix with Makefile.base.
#
#  Revision 1.1  1996/05/16 19:34:49  jussi
#  Initial revision.
#

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DAIX -I/u2/ratnakar/tcl/include -I/u2/ratnakar/tk/include
ARCH_LDFLAGS = -L/u2/ratnakar/tcl/lib -L/u2/ratnakar/tk/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.aix
