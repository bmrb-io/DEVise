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
SELECT_HTML_FILE = bmrb_select.html

.PHONY : setup
setup: s2d_props bin select_html

.PHONY : testsetup
testsetup:
	mkdir test_data; cd test_data; ln -s ../html/* .; cd ..
	mkdir test_session; cd test_session; ln -s ../dynamic_sessions/* .; \
	  cd ..
	cd cgi-bin; ln -s ../test_data data_dir; \
	  ln -s ../test_session session_dir; ln -s ../java_classes .; cd ..

.PHONY : install
install:
	./install $(DEV_DEMO_HOME) $(WWW_HOME) $(PEP_HOME) $(CGI_HOME) \
	  $(PEP_SESSION_HOME) $(PEP_DATA_HOME) $(INSTALL_MACHINE)

.PHONY : clean
clean:
	-rm $(S2D_PROPS_FILE)
	-rm -rf cgi-bin/bin
	-rm $(SELECT_HTML_FILE)

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
	echo "bmrb_mirror.summary_page_dir=$(SUMMARY_PAGE_DIR)" >> \
	  $(S2D_PROPS_FILE)

bin: cgi-bin/bin
cgi-bin/bin:
	mkdir cgi-bin/bin
	sed -e 's%java%$(JAVA_BIN)/java%' cgi-bin/java_tmpl > cgi-bin/bin/java
	chmod 755 cgi-bin/bin/java
	ln -s $(PERL_BIN)/perl cgi-bin/bin

select_html: $(SELECT_HTML_FILE)
$(SELECT_HTML_FILE): bmrb_select.tmpl
	-rm $(SELECT_HTML_FILE)
	sed -e 's%CGI_URL%$(CGI_URL)%' bmrb_select.tmpl > $(SELECT_HTML_FILE)
