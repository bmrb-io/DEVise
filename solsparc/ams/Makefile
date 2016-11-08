#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-2003
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
#  Revision 1.6.26.1  2003/12/19 18:11:00  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.6.44.1  2003/12/18 19:26:18  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.6  1998/03/04 18:41:25  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.5  1997/01/18 20:48:46  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.4  1996/11/23 20:55:53  jussi
#  Added ARCH_SYSLIBS and removed -lpthread.
#
#  Revision 1.3  1996/09/26 18:53:43  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.2  1996/05/20 18:38:24  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:07  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = $(XLIB_LIBDIR) $(TCL_TK_LIBDIR)
ARCH_SYSLIBS = $(TCL_TK_LIBS) -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../ams
include $(VPATH)/Makefile.base
include Makefile.depend
