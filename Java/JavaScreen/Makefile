#TEMPTEMP -- Makefile needs lots of changes to properly incorporate Jmol stuff
# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-2005
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

JAVAC = $(JAVA_BIN3)/javac -d . -classpath .:Jmol.jar -source 1.3 -target 1.3

all: js jsa jspop jss circ_log
js: js.class

jsa: jsa.class
jsb: jsb.class

jspop: jspop.class

jss: jss.class

circ_log: CircularLog.class

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

CircularLog.class: CircularLog.java
	$(JAVAC) CircularLog.java

utils: *.java
	$(JAVAC) DEViseCommands.java DEViseGlobals.java YError.java \
	    YException.java YGlobals.java DEViseUtils.java

client: *.java
	$(JAVAC) DEVise3DLCS.java DEViseAnimPanel.java \
	    DEViseAtomInCrystal.java DEViseAtomType.java DEViseCanvas.java \
	    DEViseCmdDispatcher.java DEViseCommCgi.java DEViseCommSocket.java \
	    DEViseComponentPanel.java DEViseCrystal.java DEViseCursor.java \
	    DEViseDebugLog.java DEViseFonts.java DEViseGData.java \
	    DEViseHeartbeat.java DEViseJSValues.java DEViseScreen.java \
	    DEViseTrafficLight.java DEViseUIGlobals.java DEViseView.java \
	    DEViseViewInfo.java YImageCanvas.java YLogGUI.java YMsgBox.java \
	    jsdevisec.java DEVisePlayback.java DEViseCollabDlg.java \
	    DEViseMainButtons.java DEViseJmolData.java DEViseCanvas2D.java \
	    DEViseCanvas3D.java DEViseCanvas3DJmol.java \
	    DEViseCanvas3DPlain.java DEViseGenericTree.java \
	    DEViseGenericTreeNode.java \
	    DEViseGenericTreeSelectionCallback.java DEViseJmolMenuButton.java

appletclass: DEViseJSApplet.java 
	$(JAVAC) DEViseJSApplet.java DEViseJSTimer.java

loader: *.java
	$(JAVAC) jsb.java DEViseJSLoader.java

jar:
	make jara jarb jarjs

jara:
	make classclean
	-rm jsa1.jar jsa2.jar
	make jsa
	make loader
	cp *.gif JavaScreen
	jar cf jsa1.jar JavaScreen/DEViseJSLoader.class JavaScreen/jsa.class
	rm JavaScreen/DEViseJSLoader.class JavaScreen/jsa.class
	jar cf jsa2.jar JavaScreen/*.class  JavaScreen/*.gif

jarb:
	make classclean
	-rm jsb1.jar jsb2.jar
	make jsb
	make loader
	cp *.gif JavaScreen
	jar cf jsb1.jar JavaScreen/DEViseJSLoader.class JavaScreen/jsb.class
	rm JavaScreen/DEViseJSLoader.class JavaScreen/jsb.class
	jar cf jsb2.jar JavaScreen/*.class  JavaScreen/*.gif

jarjs:
	make classclean
	-rm js.jar
	make js
	cp *.gif JavaScreen
	jar cf js.jar JavaScreen/*.class  JavaScreen/*.gif

#TEMPTEMP
jmol1:
	javac -classpath .:Jmol.jar Integration.java

jmol1_run:
	java -cp .:Jmol.jar Integration

jmol2:
	#TEMPTEMP $(JAVAC) -classpath .:Jmol.jar js.java
	javac -classpath .:Jmol.jar js.java

clean: classclean
	#TEMPTEMP? -rm JavaScreen/*.gif *.jar
	-rm JavaScreen/*.gif jsa1.jar jsa2.jar jsb1.jar jsb2.jar js.jar

classclean:
	-rm JavaScreen/*.class


.PHONY: setup
setup:
	./setup
	make -f Makefile.setup setup

.PHONY: install
install:
	make -f Makefile.setup install
