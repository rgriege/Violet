#!/bin/sh

dir="$1"
cd $dir
project="$2"
find . -name '*.h' | sed 's/\./	\${ROOT}/' | sort -df > ../../build/cmake/$project/headers
find . -name '*.cpp' | sed 's/\./	\${ROOT}/' | sort -df > ../../build/cmake/$project/sources
