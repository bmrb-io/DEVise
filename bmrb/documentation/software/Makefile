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
	ln -s ../../../peptide_cgi/design_doc/bmrb_setup \
		peptide-cgi_bmrb_setup.txt
	ln -s ../../../peptide_cgi/design_doc/cs_setup \
		peptide-cgi_cs_setup.txt
	ln -s ../../../peptide_cgi/design_doc/building \
		peptide-cgi_building.txt
	ln -s ../../../Java/JavaScreen/doc/commands js_commands.txt
	ln -s ../../../Java/JavaScreen/doc/js_doc js_client_doc.txt
	ln -s ../../../Java/JavaScreen/doc/jspop_doc jspop_doc.txt
	ln -s ../../../Java/JavaScreen/doc/jss_doc jss_doc.txt
	ln -s ../../../Java/JavaScreen/doc/building js_building.txt
	ln -s ../../../Java/JavaScreen/doc/adding_functionality \
		js_adding_functionality.txt
	ln -s chem_shift_install chem_shift_install.txt
	ln -s ../../../peptide_cgi/design_doc/concepts \
		peptide-cgi_concepts.txt
	ln -s chem_shift_building chem_shift_building.txt
	ln -s chem_shift_testing chem_shift_testing.txt
	ln -s chem_shift_release chem_shift_release.txt

clean:
	\rm *.txt

tar:
	cd ..; tar cvfXzh software_doc.tgz software/exclude_from_tar software
