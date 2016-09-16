#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2001-2012
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

include ../../../Makefile.config

VIS_EXAMPLE_TEMPLATE = vis_examples_all.tmpl
VIS_EXAMPLE_FILE = vis_examples_all.html

setup: $(VIS_EXAMPLE_FILE)

clean:
	-rm $(VIS_EXAMPLE_FILE)

$(VIS_EXAMPLE_FILE): $(VIS_EXAMPLE_TEMPLATE)
	-rm -f $(VIS_EXAMPLE_FILE)
	sed -e 's%CGI_URL%$(CGI_URL)%' $(VIS_EXAMPLE_TEMPLATE) > $(VIS_EXAMPLE_FILE)
