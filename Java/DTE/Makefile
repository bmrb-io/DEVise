DEBUG=-g -deprecation
CLASSES = Classes
#PACKAGE=Util
#CLASSPATH=/s/java/lib/classes.zip:../$(CLASSES)

SRC =   DTE_Type.java \
	Iterator.java \
	ListOp.java \

#    ExecExpr.java \
    
PACKAGES = Types Iterators Expressions NameToIdMaps DataSources Functions

.SUFFIXES: .java.class 

#.java.class: 
#	javac $<

all:
	javac FileScanIteratorTest.java -d $(CLASSES)

doc:
	javadoc $(PACKAGES) -d /u/d/o/donjerko/public/html/Dte/
