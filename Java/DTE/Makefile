DEBUG=-g -deprecation
CLASSES = Classes

SRC =   DTE_Type.java \
	Iterator.java \
	ListOp.java \
	ExecExpr.java \
	DataSource.java \
    
PACKAGES = Types Iterators Expressions NameToIdMaps DataSources

.SUFFIXES: .java.class 


all:
	javac FileScanIteratorTest.java -d $(CLASSES)

hj:
	javac TestType.java -d $(CLASSES)

doc:
	javadoc $(PACKAGES) -d /u/d/o/donjerko/public/html/Dte/

opt:
	javac OptimizerTest.java -d $(CLASSES)

Parser/SQLParser.java: Parser/SQLParser.jj
	cd Parser && javacc SQLParser.jj  && cd ..

parser: Parser/SQLParser.java
	javac ParserTest.java -d $(CLASSES)
