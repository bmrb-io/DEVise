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

############################################################

ARCH_FLAGS = -DSOLARIS
ARCH_LDFLAGS =
ARCH_SYSLIBS = -lm
ARCH_LDPOST  = -lsocket -lnsl
ARCH_ENDIAN =

DTE_DIR = ../../RTree
VPATH = $(DTE_DIR) $(DTE_DIR)/src $(DTE_DIR)/src/Common \
	$(DTE_DIR)/src/DBJussi $(DTE_DIR)/src/PageClasses \
	$(DTE_DIR)/src/Progs $(DTE_DIR)/src/RTree $(DTE_DIR)/src/TypeSupport

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.solaris
