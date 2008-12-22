# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2008
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

# Makefile for setting up BMRB internal documentation.

############################################################

# $Id$

############################################################

setup:
	ln -s ../../../README devise-building.txt
	ln -s ../../../peptide_cgi/design_doc/new_visualization \
		peptide-cgi_new_visualization.txt
	ln -s ../../../peptide_cgi/design_doc/auto_testing \
		peptide-cgi_auto_testing.txt
	ln -s ../../../peptide_cgi/design_doc/testing \
		peptide-cgi_testing.txt
	ln -s ../../../design_doc/release_how_to devise_release_how_to.txt
	ln -s ../../../design_doc/distribution_how_to \
		devise_distribution_how_to.txt
	ln -s ../../../peptide_cgi/design_doc/release_procedure \
		peptide-cgi_release_procedure.txt
	ln -s ../../../dist/bmrb_mirror_instructions \
		bmrb_mirror_instructions.txt
	ln -s ../../../peptide_cgi/INSTALL_NOTES.txt peptide-cgi_install.txt
	ln -s ../../../peptide_cgi/CONFIG.txt peptide-cgi_config.txt

clean:
	\rm *.txt

tar:
	cd ..; tar cvfXzh software_doc.tgz software/exclude_from_tar software
