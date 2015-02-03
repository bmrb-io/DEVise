# try ssl sockets? (see http://stackoverflow.com/questions/13874387/create-app-with-sslsocket-java)
all:
	javac SockTestClient.java
	javac SockTestServer.java
	javac SockTestClient2.java
	javac SockTestServer2.java
	javac SockTestClient3.java
	javac SockTestServer3.java
	javac SockTestClient4.java
	javac SockTestServer4.java
	javac SockTestClient5.java
	javac SockTestServer5.java
	javac SockTestClient6.java
	javac SockTestServer6.java

jar:
	jar cfm SockTestClient.jar Manifest.txt *.class

sign:
	jarsigner -tsa https://timestamp.geotrust.com/tsa -storetype pkcs12 \
		-keystore ../js_certs/uw_signing_chain.pfx \
		SockTestClient.jar \
		"university of wisconsin-madison's internet2 id"

cert:
	keytool -genkey -keystore test_cert -keyalg RSA -storepass devtest \
		-keypass devtest \
		-dname "cn=DEVise, ou=comp_sci, o=uw_madison, c=US"

install:
	mkdir -p /var/home/www/public/html/devise/test/sock_test
	cp *.jar *.html /var/home/www/public/html/devise/test/sock_test

test:
	java SockTestClient
stest:
	java SockTestServer

test2:
	java SockTestClient2
stest2:
	java SockTestServer2

test3:
	java SockTestClient3
stest3:
	java SockTestServer3

test4:
	java SockTestClient4
stest4:
	java SockTestServer4

test5:
	java SockTestClient5
stest5:
	java SockTestServer5

test6:
	java -Djavax.net.ssl.trustStore=test_cert -Djavax.net.ssl.trustStorePassword=devtest SockTestClient6
stest6:
	java -Djavax.net.ssl.keyStore=test_cert -Djavax.net.ssl.keyStorePassword=devtest -Djava.protocol.handler.pkgs=com.sun.net.ssl.internal.www.protocol -Djavax.net.debug=ssl SockTestServer6

clean:
	-\rm *.class *.jar
