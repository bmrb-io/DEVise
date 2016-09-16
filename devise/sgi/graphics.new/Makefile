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
#  Revision 1.8  1998/03/04 18:40:48  wenger
#  Added <arch>/Makefile.config files that now hold stuff like the
#  PERL_CORE definitions so those don't have to be repeated in all
#  of the lower level makefiles; various other compile fixes; changed
#  version to 1.5.2.
#
#  Revision 1.7  1998/03/02 22:32:29  wenger
#  Got DEVise to link on SGI (haha) -- had to change things so that all
#  templates are implicit (they are still not implicit on other
#  architectures, to save space), had to use GNU's ar instead of SGI's,
#  various other kludges.
#
#  Revision 1.6  1998/02/23 23:23:42  wenger
#  Merged cleanup_1_4_7_br_8 thru cleanup_1_4_7_br_9.
#
#  Revision 1.5  1998/02/20 17:03:11  wenger
#  Updated all architectures to use new dependency scheme in graphics and
#  graphics.new directories.
#
#  Revision 1.4.16.1  1998/02/23 19:53:04  wenger
#  Got DEVise 1.4.7 to compile and link on haha; fixed bug 268.
#
#  Revision 1.4  1995/12/29 18:54:39  jussi
#  Removed -fno-for-scope compiler flag.
#
#  Revision 1.3  1995/12/28 21:53:37  jussi
#  Added -I/usr/include compiler flag.
#
#  Revision 1.2  1995/12/28 20:53:36  jussi
#  Updated ARCH_FLAGS to reflect SGI platform.
#
#  Revision 1.1  1995/12/28 16:11:59  jussi
#  Initial revision.
#

############################################################

include ../Makefile.config

VPATH = ../../graphics.new

ifeq ($(DEV_DEBUG_GRAPHICS_NEW),1)
  DEV_DEBUG = 1
endif

include $(VPATH)/Makefile.base
include Makefile.depend
