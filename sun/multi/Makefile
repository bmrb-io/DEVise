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
#  Revision 1.13  1997/08/12 18:49:28  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.12  1997/01/18 20:49:16  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.11  1996/07/18 14:10:26  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.10  1996/07/13 02:39:49  jussi
#  Added -static linker option.
#
#  Revision 1.9  1996/07/13 02:18:15  jussi
#  Removed /usr/unsup/lib which appeared unnecessary.
#
#  Revision 1.8  1996/07/08 18:16:13  wenger
#  Fixed multi makefiles with ARCH_SYSLIBS macro like generic so that
#  multi will compile for HP.
#
#  Revision 1.7  1995/09/28 15:10:05  jussi
#  Added copyright and CVS log messages.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/tcl/lib
ARCH_SYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../multi
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sun
