
.SUFFIXES: .java .class
JFLAGS = -g
RM = \rm

.java.class:
	javac -deprecation $(JFLAGS) $*.java

CLASSES =	DTEWindow.java \
		Environment.java \
		PairVector.java \
		CatalogPanel.java \
		Catalog.java \
		ResultOutputer.java \
		ResultPanel.java \
		OutputerPane.java \
		AreaPrintWriter.java \
		UpperPanel.java \
		UpperComponent.java \
		ExecuteSQLPane.java \
		ShowEnvironmentsPane.java \
		SizedComponent.java \
		SizedTextArea.java \
		SizedList.java \
		ShowGestaltPane.java \
		CreateGestaltPane.java \
		ListActionPanel.java \
		CreateTablePane.java \
		CreateViewPane.java \
		MiscPane.java \
		CreateIndexPane.java \
		SingleListPanel.java \
		ODBCPane.java \
		TitledInputPanel.java \
		ScreenScale.java \
		CatalogRefresher.java \
		DialogShower.java

main: $(CLASSES:.java=.class) Makefile Environment.so execute

Environment.so:	Environment.c Environment.h
		g++ -g -Wall -c -I/s/java/include -I/s/java/include/solaris Environment.c -o Environment.so

Environment.h:	Environment.class
		javah -jni Environment

execute:	Execute.c
		g++ Execute.c -o execute

clean::
	$(RM) *.class *.so *.h

