#!/usr/bin/env bash

export CC=clang
export CXX=clang++
conan install -if build/ . || exit
cmake -B build/ || exit
cd build || exit
make -j
