#!/bin/bash

target=$1
output="bin/$1"
opts=`getopt -o o: --long output: -n 'parse-options' -- "$@"`

if [ $? != 0 ] ; then echo "Failed parsing options." >&2 ; exit 1 ; fi

eval set -- "$opts"

while true; do
  case "$1" in
    -o | --output ) output=$2; shift; shift ;;
    * ) break ;;
  esac
done

echo "output is $output"

export CC=emcc
export CXX=em++
export PATH=/usr/lib/emscripten:$PATH

flags="-s USE_SDL=2 -s USE_SDL_IMAGE=2 --use-preload-plugins"

export CXXFLAGS=$flags

emmake make $target
mv "$output" "$output.bc"
emcc $flags -O3 -s ALLOW_MEMORY_GROWTH=1 "$output.bc" -o "$output.html" --preload-file data --shell-file emscripten/shell_minimal.html
