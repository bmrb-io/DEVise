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

setup: 
	ln -s ../../html/visualization.css .
	cd peptide-cgi; make setup
	cd vis_examples; make setup

clean:
	-rm visualization.css
	cd peptide-cgi; make clean
	cd vis_examples; make clean

dist:
	mkdir histograms
	cd histograms; ln -s ../../../../../html2/devise/histograms/Makefile .; make dist
	mkdir peptide-cgi
	cd peptide-cgi; ln -s ../../../../../html2/devise/peptide-cgi/Makefile.dist .; make -f Makefile.dist dist
	mkdir vis_examples
	cd vis_examples; ln -s ../../../../../html2/devise/vis_examples/Makefile.dist .; make -f Makefile.dist dist
	mkdir figures
	cd figures; ln -s ../../../../../html2/devise/figures/*.* .
