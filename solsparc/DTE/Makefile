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

ARCH_FLAGS = -DSOLARIS -I/s/X11/include -I/s/tcl/include
ARCH_LDFLAGS =
ARCH_GUILDFLAGS = -L/s/X11/lib -L/s/tcl/lib
ARCH_SYSLIBS = -lm
ARCH_GUISYSLIBS = -ltk -ltcl -lX11 -lm
ARCH_LDPOST = -lsocket -lnsl
ARCH_ENDIAN = -DBYTE_BIG_ENDIAN -DWORD_BIG_ENDIAN


DTE_DIR = ../../DTE
VPATH = $(DTE_DIR)/Core $(DTE_DIR)/DeviseSpecific $(DTE_DIR)/StandAlone \
	$(DTE_DIR)/RTree $(DTE_DIR)/RTree/tools ../../attrproj/ $(DTE_DIR)/GUI

include $(DTE_DIR)/Makefile.base
include $(DTE_DIR)/Makefile.solaris
