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

all: set_it_up js jsa

js: set_it_up js.class
jsa: set_it_up jsa.class

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

clean:
	-rm JavaScreen/*.class
	-rm JavaScreen/UI/*.class

set_it_up:
	#mkdir -p JavaScreen/UI
	#ln -fs *gif resources html JavaScreen
	#ln -fs UI/*.java JavaScreen/UI/
	#ln -fs resources/toolbar_icons/*.default.* html
	./setup
	jar xf JmolApplet.jar
