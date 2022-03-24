#!/bin/bash

export CC=emcc
export CXX=em++
export PATH=/usr/lib/emscripten:$PATH

cppflags="-s USE_SDL=2 -s USE_SDL_IMAGE=2 -DGK_RELEASE "
ldflags=" -sMIN_WEBGL_VERSION=2  --preload-file tutos --preload-file data --shell-file emscripten/shell_minimal.html --use-preload-plugins"

export CXXFLAGS=$cppflags
export LDFLAGS=$ldflags

target=$1

emmake make verbose=1 $target
#mv "bin/$target" "bin/$target.bc"
#emcc $flags -O3 "./bin/$target.bc" -o ."/bin/$target.html" --preload-file data --shell-file emscripten/shell_minimal.html
