#!/usr/bin/env sh

HOST=127.0.0.1
PORT=13399

function connect {
    echo "connect $1:$2"
    ./cmake-build-debug/zone_client $1 $2
}

connect $HOST $PORT
