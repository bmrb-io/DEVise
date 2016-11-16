#!/bin/sh

# Helpful references:
# http://jackstromberg.com/2013/01/generating-a-pkcs12-file-with-openssl/
# http://stackoverflow.com/questions/9497719/extract-public-private-key-from-pkcs12-file-for-later-use-in-ssh-pk-authentifica
# https://support.comodo.com/index.php?/Default/Knowledgebase/Article/View/992/108/incommon-code-signing-sha-2
echo "Download the certificate chain into the file chain.crt"
openssl pkcs12 -in cert.pfx -clcerts -nokeys -out publickey.pem
openssl pkcs12 -in cert.pfx -nocerts -out privatekey.pem
openssl pkcs12 -export -name "bmrb" -out uw_signing_chain.pfx -inkey privatekey.pem -in publickey.pem -certfile chain.crt
rm privatekey.pem publickey.pem
