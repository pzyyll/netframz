#!/usr/bin/env sh

OUT_PATH=./
IMPORT_PATH=./
EXE=protoc

$EXE --cpp_out=$OUT_PATH -I$IMPORT_PATH *.proto
