CLASSROOT=classes
CLASSPATH=/s/java/lib/classes.zip:/u/w/e/weaver/java/lib/jgl3.0.0/src:/u/s/s/ssl/Work/LM/classes/

PACKAGES=Util AwtUtil LayoutEditor LayoutMgr

main:
	javac -g -classpath $(CLASSPATH) -d $(CLASSROOT) Main.java 

all: 
	@for i in $(PACKAGES); do echo --$$i; cd $$i; make $@; cd ..; done
