#!/usr/bin/env sh

HOST=127.0.0.1
PORT=23399

function connect {
    echo "connect $1:$2 $3"
    ./cmake-build-debug/robot $1 $2 $3
}

connect $HOST $PORT
