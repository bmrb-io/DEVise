# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2000-2003
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

JAVAC = /s/std/bin/javac -source 1.3 -target 1.3

default: all jar

all:
	cd star2devise; $(JAVAC) -classpath ../starlibj.jar *.java

dep:
	cd star2devise; $(JAVAC) -deprecation -classpath ../starlibj.jar *.java

jar: EDU
	jar cmf star2devise/manifest s2d.jar \
		star2devise/*.class EDU/bmrb/starlibj/*.class

EDU:
	jar xf starlibj.jar

clean:
	-rm star2devise/*.class s2d.jar
	-rm -r EDU
