DEBUG=-g -deprecation
CLASSES = Classes

JAVA_CC_OPT =

# Uncomment the following line to debug the parser

#JAVA_CC_OPT = -DEBUG_TOKEN_MANAGER=true

PACKAGES = Types Iterators Expressions Optimizer DataSources Parser

SRCS = $(PACKAGES:%=%/*.java)

.SUFFIXES: .java.class 

all: dte register

dte: Parser/SQLParser.java
	javac DTE.java $(SRCS) -d $(CLASSES)
	
register:
	javac Register.java -d $(CLASSES)

fileScanTest:
	javac FileScanIteratorTest.java -d $(CLASSES)

hj:
	javac TypeCheckTest.java -d $(CLASSES)

doc:
	javadoc $(PACKAGES) -d /u/d/o/donjerko/public/html/Dte/

opt:
	javac OptimizerTest.java -d $(CLASSES)

Parser/%.java: Parser/SQLParser.jj
	cd Parser && javacc $(JAVA_CC_OPT) SQLParser.jj   && cd ..

parser: Parser/SQLParser.java
	javac ParserTest.java Parser/*.java -d $(CLASSES)
