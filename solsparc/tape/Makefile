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

#  $Id$

#  $Log$
#  Revision 1.3.26.1  2003/12/19 18:12:26  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.3.44.1  2003/12/18 19:27:38  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.3  1998/03/04 18:41:37  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.2  1996/05/20 18:38:31  jussi
#  Replaced PENTIUM flag with SOLARIS.
#
#  Revision 1.1  1996/05/16 19:34:13  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

VPATH = ../../tape
include $(VPATH)/Makefile.base
include Makefile.depend
