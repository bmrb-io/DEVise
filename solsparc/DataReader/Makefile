#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-2003
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

############################################################

# $Id$

# $Log$
# Revision 1.2.22.1  2003/12/19 18:10:41  wenger
# Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
# Revision 1.2.40.1  2003/12/18 19:26:00  wenger
# Major cleanup of makefiles (got rid of all Makefile.linux
# Makefile.solaris dependency files); executables compiled on RH 7.2
# now have dynamic library dependencies that work on RH 9; updated
# release scripts for new dynamic library dependencies; startup scripts
# now work on RH 9.
#
# Revision 1.2  1998/07/29 16:41:25  wenger
# Added RCS Id, etc.
#

############################################################

include ../Makefile.config

ARCH_LDFLAGS =
ARCH_LDPOST  = -lsocket -lnsl

VPATH = ../../DataReader
include $(VPATH)/Makefile.base
include Makefile.depend
