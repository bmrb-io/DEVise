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

#  Makefile to generate peptide-cgi distributions.

############################################################

#  $Id$

############################################################

.PHONY : setup
setup peptide-cgi:
	mkdir peptide-cgi; cd peptide-cgi; ln -s ../Makefile Makefile.dist; \
	  make -f Makefile.dist peptide-cgi-setup; rm Makefile.dist

.PHONY : clean
clean:
	rm -rf peptide-cgi

.PHONY : dist
dist: peptide-cgi
	tar cvfzh peptide-cgi_`get_version`.tgz peptide-cgi

#  ========================================================================

.PHONEY : peptide-cgi-setup
peptide-cgi-setup:
	ln -s ../../Makefile .
	ln -s ../../Makefile.config.* .
	ln -s ../../bmrb_select.tmpl .
	mkdir cgi-bin; cd cgi-bin; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist cgi-bin-setup; rm Makefile.dist
	mkdir dynamic_sessions; cd dynamic_sessions; \
	  ln -s ../Makefile.dist .; \
	  make -f Makefile.dist dynamic_sessions-setup; rm Makefile.dist
	mkdir html; cd html; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist html-setup; rm Makefile.dist
	ln -s ../../install .
	mkdir java_classes; cd java_classes; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist java_classes-setup; rm Makefile.dist
	mkdir schema; cd schema; ln -s ../Makefile.dist .; \
	  make -f Makefile.dist schema-setup; rm Makefile.dist

.PHONEY : cgi-bin-setup
cgi-bin-setup:
	ln -s ../../../cgi-bin/make_view .
	ln -s ../../../cgi-bin/s2d .
	ln -s ../../../cgi-bin/set_modes .

.PHONEY : dynamic_sessions-setup
dynamic_sessions-setup:
	ln -s ../../../dynamic_sessions/*.base .
	ln -s ../../../dynamic_sessions/*.str .

.PHONEY : html-setup
html-setup:
	ln -s ../../../html/*.html .
	ln -s ../../../html/*.gif .
	ln -s ../../../html/*.jpg .
	ln -s ../../../html/*.js .
	ln -s ../../../html/*.base .
	ln -s ../../../html/*.dat .
	ln -s ../../../html/assignments.txt .
	ln -s ../../../html/chemshift.txt .

#TEMP -- make sure .class files exist
.PHONEY : java_classes-setup
java_classes-setup:
	ln -s ../../../java_classes/*.class .
	ln -s ../../../java_classes/*.jar .

.PHONEY : schema-setup
schema-setup:
	ln -s ../../../schema/bmrb:* .
