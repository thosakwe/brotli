#!/usr/bin/env bash
set -e
pub get
cmake .
cmake --build .
pub run test -j4
