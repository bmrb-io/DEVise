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
#  Revision 1.5.26.1  2003/12/19 18:12:01  wenger
#  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.
#
#  Revision 1.5.44.1  2003/12/18 19:27:12  wenger
#  Major cleanup of makefiles (got rid of all Makefile.linux
#  Makefile.solaris dependency files); executables compiled on RH 7.2
#  now have dynamic library dependencies that work on RH 9; updated
#  release scripts for new dynamic library dependencies; startup scripts
#  now work on RH 9.
#
#  Revision 1.5  1998/03/04 18:41:31  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.4  1996/11/19 15:29:25  wenger
#  Fixed SOLARIS define.
#
#  Revision 1.3  1996/11/18 18:14:47  donjerko
#  *** empty log message ***
#
#  Revision 1.2  1996/10/29 16:19:58  wenger
#  Fixed devread Makefiles in the architecture subdirectories to include
#  the proper dependency files.
#
#  Revision 1.1  1996/08/14 20:01:11  flisakow
#      Need a devread directory for each m/c arch.
#
#  Revision 1.1  1996/08/14 19:04:53  flisakow
#    Checkin of DevRead class, a near duplicate of the AttrProj class,
#    it is an interface to reading data from Devise files.
#
#  Revision 1.3  1996/07/18 13:58:46  jussi
#  We now produce dynamically linked executables.
#
#  Revision 1.2  1996/06/28 13:57:21  wenger
#  Forgot to change the attrproj Makefiles for all architectures.
#
#  Revision 1.1  1996/04/22 18:04:30  wenger
#  First version of "attribute projection" code.  The parser (with
#  the exception of instantiating any TData) compiles and runs.

############################################################

include ../Makefile.config

ARCH_LDFLAGS = -L/usr/unsup/lib $(TCL_TK_LIBDIRS)
ARCH_LDPOST  =

VPATH = ../../devread:../../graphics:../../graphics.new
include ../../devread/Makefile.base
include Makefile.depend
