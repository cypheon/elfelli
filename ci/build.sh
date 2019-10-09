#!/bin/sh

set -e

if [ -n "$USE_SCONS" ]; then
  scons -j3
else
  cmake . && make -j3
fi
