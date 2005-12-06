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
#  Revision 1.8.26.1  2003/12/19 18:12:16  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.8.44.1  2003/12/18 19:27:28  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.8  1998/03/04 18:41:35  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.7  1997/08/12 18:49:03  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.6  1997/01/18 20:48:48  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.5  1996/10/04 19:59:36  wenger
#  Temporarily (?) removed threads from Devise (removed -lpthread from
#  some Makefiles so Purify works); other minor cleanups.
#
#  Revision 1.4  1996/09/26 18:53:47  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.3  1996/07/08 18:16:00  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.2  1996/05/20 18:38:30  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:12  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = $(XLIB_LIBDIR) $(TCL_TK_LIBDIR)
ARCH_SYSLIBS = $(TCL_TK_LIBS) -lX11 -lm
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../multi
include $(VPATH)/Makefile.base
include Makefile.depend
