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
#  Revision 1.6  1997/04/11 18:45:41  wenger
#  Added proper static flags to generic Makefiles for various architectures.
#
#  Revision 1.5  1997/01/18 20:49:10  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/05/31 21:34:52  wenger
#  Fixed core dump in SPARC/Solaris version caused by GData buffer
#  misalignment; cleaned up generic/Makefile.base, etc., to get HP
#  version to link correctly and eliminate special Makefile.base.aix.
#
#  Revision 1.3  1996/05/31 15:45:25  jussi
#  A customized Makefile.base.aix is used in AIX.
#
#  Revision 1.2  1996/05/22 21:00:37  jussi
#  Merged Makefile.base.aix with Makefile.base.
#
#  Revision 1.1  1996/05/16 19:34:46  jussi
#  Initial revision.
#

############################################################

PERL_CORE = /s/perl/lib/CORE

ARCH_FLAGS   = -DAIX -I/u2/ratnakar/tcl/include -I/u2/ratnakar/tk/include
ARCH_LDFLAGS = -L/u2/ratnakar/tcl/lib -L/u2/ratnakar/tk/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =
ARCH_STATIC_LDPOST  = -static

VPATH = ../../generic
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.aix
