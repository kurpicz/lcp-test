#!/bin/bash

CUR_DIR=`pwd`
INSTALL_PREFIX=${HOME}
if [ $# -ge 1 ]; then
  INSTALL_PREFIX=${1}
fi

mkdir -p "${INSTALL_PREFIX}" 2> /dev/null
cd "${INSTALL_PREFIX}" > /dev/null 2>&1
if [ $? != 0 ]; then
  echo "ERROR: '${INSTALL_PREFIX}' does not exist and could not be created."
  exit 1
else
  INSTALL_PREFIX=`pwd -P`
fi

echo "Installing DivSufSoft in `${INSTALL_PREFIX}`."

cd "${CUR_DIR}/extlib/libdivsufsort"
mkdir -p build 2> /dev/null
cd build
if [ $? != 0 ]; then
  echo "ERROR: Could not create build directory for libdivsufsort"
  exit 1
fi

cmake -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" -DCMAKE_BUILD_TYPE=RELEASE ..
make
make install
