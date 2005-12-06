#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 1992-1997
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
#  Revision 1.3.26.1  2003/12/19 18:08:32  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.3.44.1  2003/12/18 19:23:40  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.3  1998/03/11 18:26:03  wenger
#  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
#  reducing include dependencies between csgroup code and the rest of
#  the code, and within the csgroup code.  (Note: running collaboration
#  doesn't work yet.)
#
#  Revision 1.2  1997/05/29 15:22:03  wenger
#  Updated hp, solaris, and sun dependencies; fixes to make new LM stuff
#  compile on linux.
#
#  Revision 1.1  1997/05/28 20:48:26  wenger
#  Added directories and makefiles for layout mananger to compile for hp,
#  linux, solsparc, and sun.
#

############################################################

include ../Makefile.config

VPATH = ../../LM
include $(VPATH)/Makefile.base
include Makefile.depend
