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

# Note: with the current cert, this is not needed.
realkey:
	cert_scripts/create_real_keystore.csh

testkey:
	cert_scripts/create_test_keystore.csh

export_test_cert:
	cert_scripts/export_test_cert.csh

print_cert:
	keytool -storetype pkcs12 -keystore js_certs/uw_signing_chain.pfx \
		-list -v

# Note: with the current cert, this is not needed.
import_cert:
	cert_scripts/import_cert.csh

# Other possible TSAs are http://timestamp.verisign.com and
# http://tsa.starfieldtech.com (see
# http://ant.apache.org/manual/Tasks/signjar.html).
sign:
	jarsigner -tsa https://timestamp.geotrust.com/tsa -storetype pkcs12 \
		-keystore js_certs/uw_signing_chain.pfx \
		devise_js.jar "university of wisconsin-madison's internet2 id"

showcerts:
	keytool -printcert -jarfile devise_js.jar

testsign:
	jarsigner -tsa https://timestamp.geotrust.com/tsa \
		-keystore js_certs/js_test.jks devise_js.jar js_test

# Show Java control panel (to import certs, etc.).
java_control:
	javaws -viewer

clean: classclean
	-rm devise_js.jar
	-rm js_version

classclean:
	-rm JavaScreen/*.class
	-rm JavaScreen/UI/*.class

.PHONY: setup
setup:
	./setup
	make -f Makefile.setup setup
	jar xf JmolApplet.jar
	# Note:  The target path here may need to change according
	# to where the actual cert files are.
	ln -s /p/devise/ext3/wenger4/certs js_certs

.PHONY: setupclean
setupclean:
	rm -r bin lib
	make -f Makefile.setup clean
	rm -r org
	rm js_certs

.PHONY: install
install:
	make -f Makefile.setup install

.PHONY: client_install
client_install:
	make -f Makefile.setup client_install

.PHONY: test_release
test_release:
	mkdir -p ../js_test/release/JavaScreen/logs
	-cd ../js_test/release; ln -s ../../../linux_rhel6/release_dir/ bin
	-cd ../js_test/release; ln -s ../../../linux_rhel6/dyn_lib .
	-cd ../js_test/release; ln -s ../../../lib .
	cd ../..; scripts/release.js `pwd`/Java/js_test
