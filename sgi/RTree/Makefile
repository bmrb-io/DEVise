#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1998
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
#  Revision 1.1  1998/02/26 18:55:08  wenger
#  Got everything to compile on haha -- still have a link problem, though.
#

############################################################

IMPLICIT_TEMPLATES = 1
AR = /usr/gnu/bin/ar

ARCH_FLAGS = -DSGI
ARCH_LDFLAGS =
ARCH_SYSLIBS =
ARCH_LDPOST  = -lnsl
ARCH_ENDIAN =

SRC_DIR = ../../RTree
VPATH = $(SRC_DIR) $(SRC_DIR)/src $(SRC_DIR)/src/Common \
	$(SRC_DIR)/src/DBJussi $(SRC_DIR)/src/PageClasses \
	$(SRC_DIR)/src/Progs $(SRC_DIR)/src/RTree $(SRC_DIR)/src/TypeSupport

include $(SRC_DIR)/Makefile.base
include $(SRC_DIR)/Makefile.sgi
