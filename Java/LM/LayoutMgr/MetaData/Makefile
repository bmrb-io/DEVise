DEBUG=-g
CLASSROOT=../../classes
PACKAGE=LayoutMgr/MetaData

CLASSPATH=/s/java/lib/classes.zip:/u/w/e/weaver/java/lib/jgl3.0.0/src:/u/s/s/ssl/Work/LM/classes/

SRC=DEViseType.java\
    DEViseObj.java \
    DEViseCategory.java \
    DEViseColor.java \
    DEViseSession.java \
    DEViseView.java \
    DEViseVisualLink.java \
    DEViseRecordLink.java \
    DEViseCursor.java \
    DEViseSymbol.java \
    DEVisePattern.java \
    DEViseOrient.java \
    DEViseSchema.java \
    DEViseMapping.java \
    DEViseTData.java \


all: $(SRC)
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) -depend $(SRC)

%.class : %.java
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) $<
