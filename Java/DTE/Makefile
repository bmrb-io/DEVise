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
	javac TypeCheckTest.java -d $(CLASSES)

doc:
	javadoc $(PACKAGES) -d /u/d/o/donjerko/public/html/Dte/

opt:
	javac OptimizerTest.java -d $(CLASSES)

Parser/SQLParser.java: Parser/SQLParser.jj
	cd Parser && javacc -DEBUG_TOKEN_MANAGER=true SQLParser.jj   && cd ..

parser: Parser/SQLParser.java
	javac ParserTest.java Parser/*.java -d $(CLASSES)
