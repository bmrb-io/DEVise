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
#  Revision 1.10  1998/03/04 18:41:47  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.9  1997/01/18 20:49:15  jussi
#  Removed inclusion of Xaw and related X extensions.
#
#  Revision 1.8  1996/11/23 20:57:51  jussi
#  Added ARCH_SYSLIBS.
#
#  Revision 1.7  1996/07/18 14:10:24  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.6  1995/09/28 15:09:57  jussi
#  Added copyright and CVS log messages.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/tcl7.4/lib
ARCH_SYSLIBS = -ltk4.0 -ltcl7.4 -lX11 -lm
ARCH_LDPOST  =

VPATH = ../../ams
include $(VPATH)/Makefile.base
include $(VPATH)/Makefile.sun
