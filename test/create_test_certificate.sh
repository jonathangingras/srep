#!/bin/bash

mkdir -p ssl

# openssl genrsa 2048 > ssl/private.pem

# openssl req -x509 -days 1000 -new -key ssl/private.pem -out ssl/public.pem

# openssl pkcs12 -export -in ssl/public.pem -inkey ssl/private.pem -out ssl/mycert.pfx

# openssl dhparam -out ssl/dh2048.pem 2048

openssl genrsa -des3 -out ssl/server.key 1024
openssl req -new -key ssl/server.key -out ssl/server.csr
openssl x509 -req -days 3650 -in ssl/server.csr -signkey ssl/server.key -out ssl/server.crt
openssl dhparam -out ssl/dh1024.pem 1024
