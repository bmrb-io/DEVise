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
#  Revision 1.1  1996/08/14 20:01:11  flisakow
#      Need a devread directory for each m/c arch.
#
#  Revision 1.1  1996/08/14 19:04:53  flisakow
#    Checkin of DevRead class, a near duplicate of the AttrProj class,
#    it is an interface to reading data from Devise files.
#
#  Revision 1.3  1996/07/18 13:58:46  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.2  1996/06/28 13:57:21  wenger
#  Forgot to change the attrproj Makefiles for all architectures.
#
#  Revision 1.1  1996/04/22 18:04:30  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.

############################################################

ARCH_FLAGS   = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_LDPOST  =

VPATH = ../../devread:../../graphics:../../graphics.new
include ../../devread/Makefile.base
include ../../devread/Makefile.solaris
