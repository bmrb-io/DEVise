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
#  Revision 1.4  1996/03/26 15:35:09  wenger
#  Fixed various compile warnings and errors; added 'clean' and
#  'mostlyclean' targets to makefiles; changed makefiles so that
#  object lists are derived from source lists.
#
#  Revision 1.3  1996/01/12 18:00:09  jussi
#  Switched to newer compiler version with all necessary header files.
#
#  Revision 1.2  1996/01/12 16:07:38  jussi
#  Gcc header files for HP-UX seem to have vanished; substituted
#  Sun header files instead.
#
#  Revision 1.1  1995/10/15 19:12:17  jussi
#  Initial revision.
#

############################################################

ARCH_FLAGS = -DHPUX -I/s/tcl/include
ARCH_LDFLAGS = -L/s/tcl/lib

VPATH = ../../graphics.new
include $(VPATH)/Makefile.base
include Makefile.depend
