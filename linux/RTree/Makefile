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
#  Revision 1.1.26.1  2003/12/19 18:08:37  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.1.44.1  2003/12/18 19:23:46  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.1  1998/03/11 18:30:52  wenger
#  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
#  reducing include dependencies between csgroup code and the rest of
#  the code, and within the csgroup code.  (Note: running collaboration
#  doesn't work yet.)
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
include Makefile.depend
