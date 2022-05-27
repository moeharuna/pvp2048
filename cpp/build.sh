#!/usr/bin/env bash

export CC=clang
export CXX=clang++
conan install -if build/ . || exit
mkdir -p src/proto_out || exit
protoc -I ../grpc/ ../grpc/game.proto --plugin=protoc-gen-grpc="$(which grpc_cpp_plugin)" --grpc_out=./src/proto_out || exit
protoc -I ../grpc/ ../grpc/game.proto --cpp_out=./src/proto_out || exit
cmake -B build/ || exit
cd build || exit
make -j
