DEBUG=-g -deprecation
CLASSES = Classes

SRC =   DTE_Type.java \
	Iterator.java \
	ListOp.java \
	ExecExpr.java \
    
PACKAGES = Types Iterators Expressions NameToIdMaps DataSources

.SUFFIXES: .java.class 


all:
	javac FileScanIteratorTest.java -d $(CLASSES)

hj:
	javac TestType.java -d $(CLASSES)

doc:
	javadoc $(PACKAGES) -d /u/d/o/donjerko/public/html/Dte/
