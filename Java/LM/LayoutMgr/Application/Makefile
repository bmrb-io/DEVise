DEBUG=-g -deprecation
CLASSROOT=../../classes
PACKAGE=LayoutMgr/Application

CLASSPATH=/s/java/lib/classes.zip:/u/w/e/weaver/java/lib/jgl3.0.0/src:/u/s/s/ssl/Work/LM/classes/:/u/s/s/ssl/Work/LM/graphicJava_1.1/

SRC= LMgr.java \
     DisplayPanel.java \
     SideCardPanel.java \
     PanelCard.java \
     VisualLinkPanel.java \
     RecordLinkPanel.java \
     CursorPanel.java \
     MappingPanel.java \
     TDataPanel.java  \
     TDataMapPanel.java \
     DEViseEditCanvas.java \
     EditorPanel.java \
     DEViseGridLayout.java \
     DEVisePileLayout.java \
     DEViseCustomLayout.java \
     DEViseLayout.java \
     DEViseLayoutState.java \
     DEViseSessionPeer.java \
     DEViseLayoutPeer.java \
     DEViseViewPeer.java \
     DEViseRecordLinkPeer.java \
     DEViseVisualLinkPeer.java \
     DEViseCursorPeer.java \
     DEViseMappingPeer.java \
     DEViseTDataPeer.java \
     EditDlg.java \
     LayoutModeDlg.java \
     LMError.java \
     Browser.java \
     SchemaBrowser.java \
     DummyCatalog.java \
     CatalogBrowser.java \
     MappingDlg.java \


all: $(SRC)
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) $(SRC)

%.class : %.java
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) $<
