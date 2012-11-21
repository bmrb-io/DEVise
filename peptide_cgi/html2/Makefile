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
	cd devise; make setup

clean:
	cd devise; make clean

dist:
	ln -s ../../../html2/bmrb_home.html .
	mkdir bmrb_home_files
	cd bmrb_home_files; ln -s ../../../../html2/bmrb_home_files/* .
	rm bmrb_home_files/CVS
	ln -s ../../../html2/README .
	mkdir devise
	cd devise; ln -s ../../../../html2/devise/Makefile .; make dist
