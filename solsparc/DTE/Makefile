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

#  $Log$
#  Revision 1.11  1998/03/04 18:41:22  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.10  1997/11/05 00:24:25  donjerko
#  *** empty log message ***
#
#  Revision 1.9  1997/08/12 18:48:51  wenger
#  Fixes to get DEVise to compile on Linux; changed -gstabs to -g in
#  makefiles; various other makefile cleanups.
#
#  Revision 1.8  1997/03/20 19:56:41  wenger
#  Minor mods to get latest version to compile on HP and SunOS; cleaned
#  up DTE makefiles (moved all targets from architecture-specific makefiles
#  to DTE/Makefile.base).
#
#  Revision 1.7  1997/03/11 22:50:28  donjerko
#  *** empty log message ***
#
#  Revision 1.6  1997/02/25 22:17:52  donjerko
#  *** empty log message ***
#
#  Revision 1.5  1997/02/03 04:14:28  donjerko
#  Catalog management moved to DTE
#
#  Revision 1.6  1996/12/16 07:22:15  donjerko
#  Fixed some compile problems
#
#  Revision 1.5  1996/12/15 06:29:43  donjerko
#  Added two binary files to support RTrees (temporary fix)
#
#  Revision 1.4  1996/11/25 04:22:02  donjerko
#  Removed "make libdevread.a"
#
#  Revision 1.3  1996/11/23 20:54:53  jussi
#  Removed -lpthread.
#
#  Revision 1.2  1996/11/23 01:53:29  donjerko
#  added standalone option
#
#  Revision 1.1  1996/11/18 21:08:05  donjerko
#  *** empty log message ***
#
############################################################

include ../Makefile.config

ARCH_LDFLAGS =
ARCH_GUILDFLAGS = -L/s/X11/lib -L/s/tcl/lib
ARCH_SYSLIBS = -lm
ARCH_GUISYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl

DTE_DIR = ../../DTE

include $(DTE_DIR)/Makefile.base
