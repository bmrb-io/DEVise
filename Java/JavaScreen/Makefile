# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-2000
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.


# Makefile for JavaScreen.

# $Id$

all: js jsa jspop jss
js: js.class

jsa: jsa.class

jspop: jspop.class

jss: jss.class

js.class: *.java
	javac js.java

jsa.class: *.java
	javac jsa.java

jspop.class: *.java
	javac jspop.java

jss.class: *.java
	javac jss.java

jar: js jsa
	jar cf jsa.jar *.class *.gif

clean:
	-rm *.class *.jar
