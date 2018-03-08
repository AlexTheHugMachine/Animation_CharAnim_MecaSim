#!/bin/bash

export CC=emcc
export CXX=em++
export PATH=/usr/lib/emscripten:$PATH

flags="-s USE_SDL=2 -s USE_SDL_IMAGE=2 --use-preload-plugins"

export CXXFLAGS=$flags

target=$1

emmake make $target
mv "bin/$target" "bin/$target.bc"
emcc $flags -O3 "./bin/$target.bc" -o ."/bin/$target.html" --preload-file data --shell-file emscripten/shell_minimal.html
