INSTALL_DIR = /var/home/www/public/html/devise/test/signing_test/Scribble

all: Scribble.java
	javac Scribble.java

jar: 
	jar cfm Scribble.jar Manifest.txt Scribble.class

clean:
	rm *.class *.jar

sign:

install:
	chmod 755 Scribble.jar
	chmod 755 Scribble.html
	mkdir -p $(INSTALL_DIR)
	cp -p Scribble.jar Scribble.html $(INSTALL_DIR)
