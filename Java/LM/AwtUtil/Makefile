DEBUG=-g -deprecation
CLASSROOT=../classes
PACKAGE=AwtUtil

CLASSPATH=/s/java/lib/classes.zip:/u/w/e/weaver/java/lib/jgl3.0.0/src:/u/s/s/ssl/Work/LM/classes/:/u/s/s/ssl/Work/LM/graphicJava_1.1

SRC=MenuMaker.java \
    NiceToolbar.java \
    ChoiceToolbar.java \
    Selectable.java \
    Rubberband.java \
    MessageDialog.java \
    CommonDialog.java \
    DualModeStickyImageButton.java \
    DualModeStickyImageButtonListener.java \
    DualModeActionListener.java \
    ToolBarMaker.java \
    ConfigureColor.java \
    ColorSelectDlg.java \
    ImageItem.java \
    EditableImageItem.java \
    ItemSelector.java \
    ItemPanel.java  \
    NiceCardPanel.java  \
    MoreGraphics.java


all: $(SRC)
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) -depend $(SRC)

TestDialog.class : TestDialog.java
	javac $(DEBUG) -classpath $(CLASSPATH)  $<

%.class : %.java
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) $<
