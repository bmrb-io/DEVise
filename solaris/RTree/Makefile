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

#  $Id$

#  $Log$
#  Revision 1.5  1998/03/02 22:33:40  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.4  1998/02/26 17:20:06  wenger
#  Fixed problems with yesterday's commit.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS =
ARCH_SYSLIBS = -lm
ARCH_LDPOST  = -lsocket -lnsl

SRC_DIR = ../../RTree
VPATH = $(SRC_DIR) $(SRC_DIR)/src $(SRC_DIR)/src/Common \
	$(SRC_DIR)/src/DBJussi $(SRC_DIR)/src/PageClasses \
	$(SRC_DIR)/src/Progs $(SRC_DIR)/src/RTree $(SRC_DIR)/src/TypeSupport

include $(SRC_DIR)/Makefile.base
include $(SRC_DIR)/Makefile.solaris
