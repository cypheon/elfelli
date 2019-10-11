#!/bin/sh

set -e

if [ "$BUILDSYSTEM" = "scons" ]; then
  scons -j3
else
  cmake . && make -j3
fi
