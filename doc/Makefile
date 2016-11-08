# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-1997
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

############################################################

# $Id$

# $Log$

############################################################

.SUFFIXES:	.tex .ps

# .tex file needs to be latex'd twice so that internal
# document references (page numbers, section numbers, etc.)
# can be resolved

.tex.ps:
	latex $*
	latex $*
	dvips $*

OUTPUT_FILES = api.ps sysman.ps userman.ps 3d-doc.ps
TEMP_FILES = 3d-doc.dvi api.dvi sysman.dvi userman.dvi

all: $(OUTPUT_FILES)

clean:
	-rm $(OUTPUT_FILES)
	-rm $(TEMP_FILES)
