DEBUG=-g -deprecation
CLASSES = Classes
#PACKAGE=Util
#CLASSPATH=/s/java/lib/classes.zip:../$(CLASSES):$(CLASSES)/test:.

SRC =   DTE_Type.java \
	Iterator.java \
	ListOp.java \

#    ExecExpr.java \
    
PACKAGES = Types Iterators Expressions NameToIdMaps DataSources Functions

.SUFFIXES: .java.class 

#.java.class: 
#	javac $<

dte:
	javac DTE.java */*.java -d $(CLASSES)

Parser/SQLParser.java: Parser/SQLParser.jj
	cd Parser; javacc SQLParser.jj; cd ..

all:
	javac DataSources/RelationManager.java -d $(CLASSES)

hj:
	javac TestExpr.java -d $(CLASSES)

doc:
	javadoc $(PACKAGES) -d /u/d/o/donjerko/public/html/Dte/
