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
#  Revision 1.7  1996/11/05 19:46:55  wenger
#  Fixed some bugs in the attribute projection code (handles blanks lines
#  and much longer lines in projection files); removed unneeded -lpthread
#  from Makefile for solaris attribute projection.
#
#  Revision 1.6  1996/09/26 18:54:00  jussi
#  Added -lpthread to get thread support.
#
#  Revision 1.5  1996/07/18 14:27:48  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.4  1996/06/27 18:13:16  wenger
#  Re-integrated most of the attribute projection code (most importantly,
#  all of the TData code) into the main code base (reduced the number of
#  modules used only in attribute projection).
#
#  Revision 1.3  1996/06/07 19:41:19  wenger
#  Integrated some of the special attribute projection sources back
#  into the regular Devise sources.
#
#  Revision 1.2  1996/05/20 18:37:56  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/04/22 18:05:34  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.

############################################################

ARCH_FLAGS   = -DSOLARIS -I/s/tcl/include
ARCH_LDFLAGS = -L/usr/unsup/lib -L/s/tcl/lib
ARCH_LDPOST  = 

VPATH = ../../attrproj:../../graphics:../../graphics.new
include ../../attrproj/Makefile.base
include ../../attrproj/Makefile.solaris
