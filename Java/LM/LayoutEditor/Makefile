DEBUG=-g -deprecation
CLASSROOT=../classes
PACKAGE=LayoutMgr/LayoutEditor

CLASSPATH=/s/java/lib/classes.zip:/u/w/e/weaver/java/lib/jgl/src:/u/s/s/ssl/Work/LM/classes/

SRC= EditCanvas.java \
    Box.java \
    Edge.java \
    Node.java \
    GroupBox.java \
    RivetBar.java \
    LayoutState.java 

all: $(SRC) 
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) -depend $(SRC)

TestEditor.class : $(SRC)
	javac $(DEBUG) -classpath $(CLASSPATH) -depend TestEditor.java

%.class : %.java
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) $<
