all:
	# Plain socket w/ DataOutputStream/DataInputStream; writes
	# actual JavaScreen command and reads (part of) the response.
	javac SockTestClient.java
	javac SockTestServer.java

	# Plain socket w/ DataOutputStream/DataInputStream; writes
	# shorts & ints.
	javac SockTestClient2.java
	javac SockTestServer2.java

	# Plain socket w/ BufferedOutputStream/BufferedInputStream;
	# writes bytes.
	javac SockTestClient3.java
	javac SockTestServer3.java

	# Plain socket w/ ObjectOutputStream/ObjectInputStream; writes
	# shorts & ints.
	javac SockTestClient4.java
	javac SockTestServer4.java

	# Plain socket w/ OutputStream/InputStream; writes bytes.
	javac SockTestClient5.java
	javac SockTestServer5.java

	# SSLSocket w/ BufferedOutputStream/BufferedInputStream; writes
	# bytes.
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
	java SockTestClient6
stest6:
	java SockTestServer6

clean:
	-\rm *.class *.jar
