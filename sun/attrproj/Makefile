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
#  Revision 1.4  1998/03/04 18:41:48  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.3  1996/07/18 14:10:25  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.2  1996/06/28 13:57:34  wenger
#  Forgot to change the attrproj Makefiles for all architectures.
#
#  Revision 1.1  1996/04/22 18:03:55  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl7.4/lib
ARCH_LDPOST  =

VPATH = ../../attrproj:../../graphics:../../graphics.new
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.sun
