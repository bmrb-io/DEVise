DEBUG=-g -deprecation
CLASSROOT=../classes
PACKAGE=Util
CLASSPATH=/s/java/lib/classes.zip:/u/w/e/weaver/java/lib/jgl3.0.0/src:/u/s/s/ssl/Work/LM/classes/

SRC=Tracer.java \
    PrintObjectOutput.java \
    ReadObjectInput.java \
    AsciiSerializable.java 
    
all: $(SRC)
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) -depend $(SRC)

%.class : %.java
	javac $(DEBUG) -classpath $(CLASSPATH) -d $(CLASSROOT) $<
