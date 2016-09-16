# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 2000-2015
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

JAVAC = javac -source 1.7 -target 1.7

default: all jar

all:
	cd EDU/bmrb/starlibj; $(JAVAC) *.java
	cd star2devise; $(JAVAC) -classpath .. *.java

dep:
	cd star2devise; $(JAVAC) -deprecation -classpath .. *.java

jar: 
	jar cmf star2devise/manifest s2d.jar \
		star2devise/*.class EDU/bmrb/starlibj/*.class

EDU:
	jar xf starlibj.jar

clean:
	-rm star2devise/*.class EDU/bmrb/starlibj/*.class s2d.jar
