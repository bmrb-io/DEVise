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
	javac *.java

jsa.class: *.java
	javac *.java

jspop.class: *.java
	javac *.java

jss.class: *.java
	javac *.java

jar:
	make jara jarb

jara:
	-rm *.class
	-rm jsa1.jar jsa2.jar
	javac DEViseJSLoader.java
	javac jsa.java
	jar cf jsa1.jar DEViseJSLoader.class jsa.class
	rm DEViseJSLoader.class jsa.class
	jar cf jsa2.jar *.class  *.gif

jarb:
	-rm *.class
	-rm jsb1.jar jsb2.jar
	javac DEViseJSLoader.java
	javac jsb.java
	jar cf jsb1.jar DEViseJSLoader.class jsb.class
	rm DEViseJSLoader.class jsb.class
	jar cf jsb2.jar *.class  *.gif

clean:
	-rm *.class *.jar
