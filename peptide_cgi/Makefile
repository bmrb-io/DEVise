#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2001
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

#  Makefile to set up peptide-cgi.

############################################################

#  $Id$

############################################################

include Makefile.config

S2D_PROPS_FILE = cgi-bin/s2d.props

.PHONY : setup
setup: s2d_props
	#TEMP -- set up bin directory, etc.

.PHONY : testsetup
testsetup:
	mkdir test_data; cd test_data; ln -s ../html/* .; cd ..
	mkdir test_session; cd test_session; ln -s ../dynamic_sessions/* .; \
	  cd ..
	cd cgi-bin; ln -s ../test_data data_dir; \
	  ln -s ../test_session session_dir; ln -s ../java_classes .; cd ..

.PHONY : install
install:
	#TEMP -- do actual installation

.PHONY : clean
clean:
	-rm $(S2D_PROPS_FILE)
	#TEMP -- remove up bin directory, etc.

.PHONY : testclean
testclean:
	-rm -rf test_data test_session
	-rm cgi-bin/data_dir cgi-bin/session_dir cgi-bin/java_classes

#  ========================================================================

.PHONEY : s2d_props
s2d_props:
	-rm $(S2D_PROPS_FILE)
	echo "# Configuration-specific s2d properties" > $(S2D_PROPS_FILE)
	echo "bmrb_mirror.star_url=$(STAR_URL)" >> $(S2D_PROPS_FILE)
	echo "bmrb_mirror.3d_url=$(3D_EXAMPLE_URL)" >> $(S2D_PROPS_FILE)
