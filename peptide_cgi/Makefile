#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2001-2003
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

# Defaults -- can be overridden in Makefile.config.
ERROR_EMAIL = devise@cs.wisc.edu
JAR_HOME = null
DO_CSR_DEFAULT = 0

include Makefile.config

S2D_PROPS_FILE = cgi-bin/s2d.props
ERROR_EMAIL_FILE = cgi-bin/error_email
SELECT_HTML_FILE = bmrb_select.html

setup: s2d_props bin select_html error_email tmp

testsetup:
	mkdir test_html; cd test_html; ln -s ../html/* .
	mkdir test_data; cd test_data; ln -s ../data/* .
	mkdir test_session; cd test_session; ln -s ../dynamic_sessions/* .
	cd cgi-bin; ln -s ../test_html html_dir
	cd cgi-bin; ln -s ../test_data data_dir
	cd cgi-bin; ln -s ../test_session session_dir
	cd cgi-bin; ln -s ../chem_info .
	cd cgi-bin; ln -s ../session_templates .
	cd cgi-bin; ln -s ../html_templates .
	cd cgi-bin; ln -s ../src/s2d.jar .
	mkdir test_data/vis_server
	mkdir test_session/vis_server

# Goofiness here is because we don't want to use Gnu make's .PHONY feature.
install: do_install
do_install: 
	./install $(DEV_DEMO_HOME) $(WWW_HOME) $(PEP_HOME) $(CGI_HOME) \
	  $(PEP_SESSION_HOME) $(PEP_DATA_HOME) $(PEP_HTML_HOME) \
	  $(JAR_HOME) $(INSTALL_MACHINE)

clean:
	-rm $(S2D_PROPS_FILE)
	-rm -rf cgi-bin/bin
	-rm -rf cgi-bin/tmp
	-rm $(SELECT_HTML_FILE)
	-rm $(ERROR_EMAIL_FILE)

testclean:
	-rm -rf test_data test_session test_html
	-rm cgi-bin/data_dir cgi-bin/session_dir cgi-bin/s2d.jar
	-rm cgi-bin/html_dir
	-rm cgi-bin/session_templates cgi-bin/chem_info cgi-bin/html_templates
	-rm -rf test_data/vis_server

#  ========================================================================

s2d_props:
	-rm -f $(S2D_PROPS_FILE)
	echo "# Configuration-specific s2d properties" > $(S2D_PROPS_FILE)
	echo "bmrb_mirror.star_url=$(STAR_URL)" >> $(S2D_PROPS_FILE)
	echo "bmrb_mirror.pdb_file_url=$(PDB_FILE_URL)" >> $(S2D_PROPS_FILE)
	echo "bmrb_mirror.3d_url=$(3D_EXAMPLE_URL)" >> $(S2D_PROPS_FILE)
	echo "bmrb_mirror.summary_page_dir=$(SUMMARY_PAGE_DIR)" >> \
	  $(S2D_PROPS_FILE)
	echo "bmrb_mirror.cgi_url=$(CGI_URL)" >> $(S2D_PROPS_FILE)
	echo "bmrb_mirror.do_csr_default=$(DO_CSR_DEFAULT)" >> $(S2D_PROPS_FILE)

error_email:
	-rm -f $(ERROR_EMAIL_FILE)
	echo $(ERROR_EMAIL) > $(ERROR_EMAIL_FILE)

bin: cgi-bin/bin
cgi-bin/bin:
	mkdir cgi-bin/bin
	sed -e 's%java%$(JAVA_BIN)/java%' cgi-bin/java_tmpl > cgi-bin/bin/java
	chmod 755 cgi-bin/bin/java
	ln -s $(PERL_BIN)/perl cgi-bin/bin

tmp: cgi-bin/tmp
cgi-bin/tmp:
	mkdir cgi-bin/tmp

select_html: $(SELECT_HTML_FILE)
$(SELECT_HTML_FILE): bmrb_select.tmpl
	-rm -f $(SELECT_HTML_FILE)
	sed -e 's%CGI_URL%$(CGI_URL)%' bmrb_select.tmpl > $(SELECT_HTML_FILE)
