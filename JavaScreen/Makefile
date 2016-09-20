# ========================================================================
# DEVise Data Visualization Software
# (c) Copyright 1992-2016
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

JAVAC = $(JAVA_BIN3)/javac -d . -source 1.7 -target 1.7

all: js jsa
js: js.class

jsa: jsa.class

js.class: *.java utils client
	$(JAVAC) js.java

jsa.class: *.java utils client appletclass
	$(JAVAC) jsa.java

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
    DEViseGenericTreeSelectionCallback.java DEViseJmolMenuButton.java \
    DEViseButton.java DEViseJSApplet.java DEViseJSTimer.java \
    UI/DEViseUIManager.java UI/DEViseStatusPanel.java \
    UI/DEViseMenuPanel.java UI/DEViseStatusMessage.java UI/DEViseToolBar.java \
    UI/DEViseToolBarButton.java UI/DEViseToolBarButtonAction.java \
    UI/DEViseMouseCursor.java UI/DEViseHtmlWindow.java \
    DEViseScreenResizeHandler.java DEViseSessionMenuButton.java

appletclass: DEViseJSApplet.java
	$(JAVAC) DEViseJSApplet.java DEViseJSTimer.java

jar:
	cp Manifest.txt Manifest_real.txt
	make jara
	./js -version > js_version

# Jar file with all-permissions instead of sandbox -- for testing only
jar_all:
	cp Manifest_all.txt Manifest_real.txt
	make jara
	./js -version > js_version

jara:
	make classclean
	-rm devise_js.jar
	make js jsa jsb
	jar cfm devise_js.jar Manifest_real.txt \
	    JavaScreen/*.class JavaScreen/UI/*.class \
 	    JavaScreen/*.gif JavaScreen/resources/toolbar_icons/*.png \
 	    JavaScreen/resources/toolbar_cursors/*.png \
	    JavaScreen/html/*.html \
	    JmolApplet.class \
	    org/jmol/*/*.class org/jmol/*/*/*.class org/jmol/*/*/*/*.class \
	    org/jmol/*/*/*.txt org/jmol/*/*.properties \
	    javax/*/*.class

clean:
	-rm -rf bin2 bin2p
	-rm demo public
	-rm devise_js.jar
	-rm js_version
	-rm JavaScreen/*.class
	-rm JavaScreen/UI/*.class

bin2:
	mkdir bin2
	# java needs special treatment for yola (SGI).
	echo $(JAVA_BIN2)/java > bin2/java
	ln -s $(PERL_BIN2)/perl bin2

# Note: bin2p is for JSPoP area.
bin2p:
	mkdir bin2p
	# java needs special treatment for yola (SGI).
	echo $(JAVA_BIN2P)/java > bin2p/java
	ln -s $(PERL_BIN2P)/perl bin2p

.PHONY: setup
setup: bin2 bin2p
	./setup
	jar xf JmolApplet.jar
