#!/bin/sh

GTEST_BIN="$1"
BIN_DIR=$(dirname "${GTEST_BIN}")
CA_CERT="$2"

function test_bin() {
    local CURL_ARGS=$@

    ${GTEST_BIN} &
    sleep 1
    curl -q 'https://localhost:8080' --cacert ${CA_CERT} ${CURL_ARGS[@]}
}

function check_and_cleanup() {
    if ! diff output.bin data.bin; then
        rm -f output.bin data.bin
        exit 1
    fi
    rm -f output.bin data.bin
}

echo -n "some-data-from-curl" > data.bin
args=(-d "$(cat data.bin)")
test_bin ${args[@]}
check_and_cleanup

"${BIN_DIR}/generate_random_bin"
args=(--data-binary @data.bin)
test_bin ${args[@]}
sleep 1
check_and_cleanup
