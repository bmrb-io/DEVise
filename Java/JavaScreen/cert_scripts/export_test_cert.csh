#! /bin/csh -f
cd js_certs
keytool -exportcert -alias js_test -keystore js_test.jks \
    -file js_test.cer
