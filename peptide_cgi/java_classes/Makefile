# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2000
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.

# Makefile for classes to get data from starfiles.

############################################################

# $Id$

############################################################

include ../Makefile.config

JAVAC = $(JAVA_BIN)/javac

all:
	$(JAVAC) -classpath ./starlibj.jar *.java

dep:
	$(JAVAC) -deprecation -classpath ./starlibj.jar *.java

clean:
	-rm *.class
