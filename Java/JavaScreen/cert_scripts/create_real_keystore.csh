#! /bin/csh -f
cd js_certs
keytool -genkey -keyalg RSA -alias js_real -keystore js_real.jks \
    -validity 720 -keysize 2048
keytool -certreq -alias js_real -file js_real.csr -keystore js_real.jks
