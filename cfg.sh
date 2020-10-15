#!/usr/bin/env bash

PATH=/c/Progra~1/LLVM/bin:/c/Progra~1/NASM:/c/Progra~1/meson:/c/Progra~2/Micros~2/2019/Enterprise/VC/Tools/MSVC/14.27.29110/bin/Hostx64/x64:$PATH
export PATH

CC=clang
CXX=clang
LD=lld-link
CCAR=lld-link
CFLAGS='-I$(DPDK_INC_DIR)/wpdk -I$(DPDK_INC_DIR)'
CXXFLAGS='-I$(DPDK_INC_DIR)/wpdk -I$(DPDK_INC_DIR)'
LDFLAGS='-L$(CONFIG_DPDK_DIR)/lib'
export CC CXX CFLAGS CXXFLAGS LD LDFLAGS CCAR

./configure --with-dpdk=C:/Users/Nick/Repos/build --enable-debug --without-isal --without-vhost  --without-virtio
