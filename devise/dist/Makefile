# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2001
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

# Makefile to generate DEVise distributions.

############################################################

# $Id$

############################################################

include Makefile.config

.PHONY : setup
setup:
	scripts/setup $(BASE_DIR) $(HTML_DIR)

.PHONY : clean
clean:
	scripts/clean

.PHONY : dist
dist:
	scripts/gen_dist $(FTP_DIR)
