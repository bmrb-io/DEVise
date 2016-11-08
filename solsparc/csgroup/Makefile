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
#  Revision 1.2.26.2  2003/12/19 18:11:49  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.2.26.1.2.1  2003/12/18 19:26:40  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.2.26.1  2003/07/23 22:17:42  wenger
#  DEVise now compiles on Solaris 2.8 (blackbird) instead of 2.7 (cardinal);
#  may still need some mods to get the distribution right.
#
#  Revision 1.2  1998/03/04 18:41:29  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.1  1998/02/13 15:02:14  wenger
#  Added csgroup directory and makefile.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = $(XLIB_LIBDIR) $(TCL_TK_LIBDIR)
ARCH_SYSLIBS = $(TCL_TK_LIBS) -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl
#ARCH_COLLECTOR	= /usr/ccs/bin/ld

VPATH = ../../csgroup
include $(VPATH)/Makefile.base
include Makefile.depend
