#!/bin/bash
set -e

rm -rf build_examples
mkdir -p build_examples

cd build_examples
cmake ../examples
make
