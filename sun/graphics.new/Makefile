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
#  Revision 1.8  1998/02/20 17:03:46  wenger
#  Updated all architectures to use new dependency scheme in graphics and
#  graphics.new directories.
#
#  Revision 1.7  1996/03/26 15:35:39  wenger
#  Fixed various compile warnings and errors; added 'clean' and
#  'mostlyclean' targets to makefiles; changed makefiles so that
#  object lists are derived from source lists.
#
#  Revision 1.6  1995/09/28 15:10:02  jussi
#  Added copyright and CVS log messages.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/s/tcl/lib

VPATH = ../../graphics.new
include $(VPATH)/Makefile.base
include Makefile.depend
