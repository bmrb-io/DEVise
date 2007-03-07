#  ========================================================================
#  DEVise Data Visualization Software
#  (c) Copyright 2001-2006
#  By the DEVise Development Group
#  Madison, Wisconsin
#  All Rights Reserved.
#  ========================================================================
#
#  Under no circumstances is this software to be copied, distributed,
#  or altered in any way without prior permission from the DEVise
#  Development Group.

#  Makefile to generate peptide-cgi distributions.

############################################################

#  $Id$

############################################################

.PHONY : setup
setup peptide-cgi:
	mkdir peptide-cgi; cd peptide-cgi; ln -s ../Makefile Makefile.dist; \
	  make -f Makefile.dist peptide-cgi-setup; rm Makefile.dist
	ln -s peptide-cgi peptide-cgi_`get_version`

.PHONY : clean
clean:
	-rm peptide-cgi_`get_version`
	-rm -rf peptide-cgi

.PHONY : dist
dist: peptide-cgi
	gen_dist

#  ========================================================================

.PHONEY : peptide-cgi-setup
peptide-cgi-setup:
	ln -s ../../INSTALL_NOTES.txt .
	ln -s ../../CONFIG.txt .
	ln -s ../../Makefile .
	ln -s ../../Makefile.config.* .
	ln -s ../../bmrb_select.tmpl .
	ln -s ../../win_tar .
	ln -s ../../view_entry .
	ln -s ../../scripts .
	ln -s ../../pep-version-history .
	mkdir cgi-bin; cd cgi-bin; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist cgi-bin-setup; rm Makefile.dist
	mkdir chem_info; cd chem_info; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist chem_info-setup; rm Makefile.dist
	mkdir data; cd data; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist data-setup; rm Makefile.dist
	mkdir dynamic_sessions; cd dynamic_sessions; \
	  ln -s ../Makefile.dist .; \
	  make -f Makefile.dist dynamic_sessions-setup; rm Makefile.dist
	mkdir html; cd html; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist html-setup; rm Makefile.dist
	mkdir html_templates; cd html_templates; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist html_templates-setup; rm Makefile.dist
	ln -s ../../install .
	ln -s ../../install_uvd .
	mkdir session_templates; cd session_templates; \
	  ln -s ../Makefile.dist .; \
	  make -f Makefile.dist session_templates-setup; rm Makefile.dist
	mkdir src; cd src; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist src-setup; rm Makefile.dist
	mkdir schema; cd schema; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist schema-setup; rm Makefile.dist
	find . -name CVS -exec \rm -rf {} \;

.PHONEY : cgi-bin-setup
cgi-bin-setup:
	ln -s ../../../cgi-bin/make_view .
	ln -s ../../../cgi-bin/make_uvd .
	ln -s ../../../cgi-bin/s2d .
	ln -s ../../../cgi-bin/vsc .
	ln -s ../../../cgi-bin/vur .
	ln -s ../../../cgi-bin/uvd .
	ln -s ../../../cgi-bin/view_unreleased .
	ln -s ../../../cgi-bin/set_modes .
	ln -s ../../../cgi-bin/java_tmpl .
	ln -s ../../../cgi-bin/test_all .
	ln -s ../../../cgi-bin/usage .
	ln -s ../../../cgi-bin/get_version .
	ln -s ../../../cgi-bin/get_version_cgi .
	mkdir tests
	cd tests; ln -s ../../../../cgi-bin/tests/* .
	mkdir test_data
	cd test_data; ln -s ../../../../cgi-bin/test_data/* .

.PHONEY : chem_info-setup
chem_info-setup:
	ln -s ../../../chem_info/* .

.PHONEY : data-setup
data-setup:
	ln -s ../../../data/* .

.PHONEY : dynamic_sessions-setup
dynamic_sessions-setup:
	ln -s ../../../dynamic_sessions/*.str .

.PHONEY : html-setup
html-setup:
	ln -s ../../../html/*.html .
	ln -s ../../../html/*.gif .
	ln -s ../../../html/*.jpg .
	ln -s ../../../html/*.js .
	ln -s ../../../html/*.txt .

.PHONEY : html_templates-setup
html_templates-setup:
	ln -s ../../../html_templates/* .

.PHONEY : session_templates-setup
session_templates-setup:
	ln -s ../../../session_templates/* .

#TEMP -- make sure .jar file exists
.PHONEY : src-setup
src-setup:
	ln -s ../../../src/s2d.jar .

.PHONEY : schema-setup
schema-setup:
	ln -s ../../../schema/bmrb-* .
