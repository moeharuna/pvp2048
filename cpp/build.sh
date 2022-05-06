#!/usr/bin/env bash

conan install -if build/ . || exit
cmake -B build/ || exit
cd build || exit
make -j
