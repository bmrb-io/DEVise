# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2002
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.


# Makefile for star2ad.

############################################################

# $Id$

############################################################

JAVAC = javac -target 1.1

all: Star2AdMain.class

Star2AdMain.class: *.java
	$(JAVAC) -classpath ./starlibj.jar *.java

clean:
	-rm *.class
