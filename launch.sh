#!/bin/bash

APP_NAME='tape_structure'
APP_DIR=$(dirname "$0")/$APP_NAME
APP_DIR_BUILD=$APP_DIR/build

FILE_PATH=$(dirname "$0")/tape_structure.txt

cmake -DCMAKE_BUILD_TYPE=Relaese -S "$APP_DIR" -B "$APP_DIR_BUILD" &&
  cmake --build "$APP_DIR_BUILD" &&
  "$APP_DIR_BUILD/bin/$APP_NAME" "$FILE_PATH" &&
  rm -rf "$APP_DIR_BUILD"
