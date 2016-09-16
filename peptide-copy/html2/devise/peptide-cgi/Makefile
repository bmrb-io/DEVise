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

SELECT_HTML_FILE = bmrb_select.html
SELECT2_HTML_FILE = bmrb_select2.html
RESTRAINT_SELECT_HTML_FILE = restraint_select.html

setup: select_html select2_html restraint_select
	ln -s ../../../test_html html

clean:
	-rm $(SELECT_HTML_FILE) $(SELECT2_HTML_FILE) \
		$(RESTRAINT_SELECT_HTML_FILE)
	-rm html

select_html: $(SELECT_HTML_FILE)
$(SELECT_HTML_FILE): bmrb_select.tmpl
	-rm -f $(SELECT_HTML_FILE)
	sed -e 's%CGI_URL%$(CGI_URL)%' bmrb_select.tmpl > $(SELECT_HTML_FILE)

select2_html: $(SELECT2_HTML_FILE)
$(SELECT2_HTML_FILE): bmrb_select2.tmpl
	-rm -f $(SELECT2_HTML_FILE)
	sed -e 's%CGI_URL%$(CGI_URL)%' bmrb_select2.tmpl > $(SELECT2_HTML_FILE)

restraint_select: $(RESTRAINT_SELECT_HTML_FILE)
$(RESTRAINT_SELECT_HTML_FILE): restraint_select.tmpl
	-rm -f $(RESTRAINT_SELECT_HTML_FILE)
	sed -e 's%CGI_URL%$(CGI_URL)%' restraint_select.tmpl > $(RESTRAINT_SELECT_HTML_FILE)
