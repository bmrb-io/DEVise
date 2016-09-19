#! /bin/csh -f
cd js_certs
keytool -genkey -keyalg RSA -alias js_test -keystore js_test.jks \
    -validity 360 -keysize 2048
