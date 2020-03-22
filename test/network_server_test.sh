#!/bin/sh

$1 &

sleep 1

curl -q 'https://localhost:8080' -d "some data from curl" --cacert $2
