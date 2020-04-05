#!/bin/sh

SERVER_BIN="$1"
CLIENT_BIN="$2"

"${SERVER_BIN}" &

sleep 1

"${CLIENT_BIN}"

sleep 1

if ! diff data.bin output.bin; then
    rm -f data.bin output.bin
    exit 1
fi

rm -f data.bin output.bin
