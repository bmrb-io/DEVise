DEBUG=-g -deprecation
CLASSES = Classes
#PACKAGE=Util
CLASSPATH=/s/java/lib/classes.zip:../$(CLASSES)

SRC=DTE_Type.java \
    Iterator.java

.SUFFIXES: .java.class 

#.java.class: 
#	javac $<

all:
	javac FileScanIteratorTest.java */*.java -d $(CLASSES)

doc:
	javadoc Types Iterators -d Doc
