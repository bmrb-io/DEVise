# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-2001
# By the DEVise Development Group
# Madison, Wisconsin
# All Rights Reserved.
# ========================================================================
#
# Under no circumstances is this software to be copied, distributed,
# or altered in any way without prior permission from the DEVise
# Development Group.


# Makefile for JavaScreen.

############################################################

# $Id$

############################################################

include Makefile.config

JAVAC = $(JAVA_BIN)/javac -d .

all: js jsa jspop jss
js: js.class

jsa: jsa.class
jsb: jsb.class

jspop: jspop.class

jss: jss.class

js.class: *.java utils client
	$(JAVAC) js.java

jsa.class: *.java utils client appletclass
	$(JAVAC) jsa.java

jsb.class: *.java utils client appletclass
	$(JAVAC) jsb.java

jspop.class: *.java
	$(JAVAC) *.java

jss.class: *.java utils
	$(JAVAC) jss.java

utils: *.java
	$(JAVAC) DEViseCommands.java DEViseGlobals.java YError.java \
	    YException.java YGlobals.java

client: *.java
	$(JAVAC) DEVise3DLCS.java DEViseAnimPanel.java \
	    DEViseAtomInCrystal.java DEViseAtomType.java DEViseCanvas.java \
	    DEViseCmdDispatcher.java DEViseCommCgi.java DEViseCommSocket.java \
	    DEViseComponentPanel.java DEViseCrystal.java DEViseCursor.java \
	    DEViseDebugLog.java DEViseFonts.java DEViseGData.java \
	    DEViseHeartbeat.java DEViseJSValues.java DEViseScreen.java \
	    DEViseTrafficLight.java DEViseUIGlobals.java DEViseView.java \
	    DEViseViewInfo.java YImageCanvas.java YLogGUI.java YMsgBox.java \
	    jsdevisec.java

appletclass: DEViseJSApplet.java
	$(JAVAC) DEViseJSApplet.java

loader: *.java
	$(JAVAC) DEViseJSLoader.java

jar:
	make jara jarb

jara:
	-rm JavaScreen/*.class
	-rm jsa1.jar jsa2.jar
	make jsa
	make loader
	cp *.gif JavaScreen
	jar cf jsa1.jar JavaScreen/DEViseJSLoader.class JavaScreen/jsa.class
	rm JavaScreen/DEViseJSLoader.class JavaScreen/jsa.class
	jar cf jsa2.jar JavaScreen/*.class  JavaScreen/*.gif

jarb:
	-rm JavaScreen/*.class
	-rm jsb1.jar jsb2.jar
	make jsb
	make loader
	cp *.gif JavaScreen
	jar cf jsb1.jar JavaScreen/DEViseJSLoader.class JavaScreen/jsb.class
	rm JavaScreen/DEViseJSLoader.class JavaScreen/jsb.class
	jar cf jsb2.jar JavaScreen/*.class  JavaScreen/*.gif

clean:
	-rm JavaScreen/*.class JavaScreen/*.gif *.jar

.PHONY: setup
setup:
	./setup
	make -f Makefile.setup setup

.PHONY: install
install:
	make -f Makefile.setup install
