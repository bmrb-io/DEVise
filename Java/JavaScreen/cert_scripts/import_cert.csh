#! /bin/csh -f
cd js_certs
keytool -importcert -trustcacerts -alias js_real -file \
    uw_software_cert.p12 -keystore js_real.jks
