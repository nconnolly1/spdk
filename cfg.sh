#!/usr/bin/env bash

PATH=/c/Progra~1/LLVM/bin:/c/Progra~1/NASM:/c/Progra~1/meson:$PATH
export PATH

CC=clang
CXX=clang
LD=lld-link
CCAR=lld-link
CFLAGS='-I$(SPDK_ROOT_DIR)/include/windows -I$(SPDK_ROOT_DIR)/../CUnit/include -I$(SPDK_ROOT_DIR)/../wpdk/include -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS'
CXXFLAGS='-I$(SPDK_ROOT_DIR)/include/windows -I$(SPDK_ROOT_DIR)/../CUnit/include -I$(SPDK_ROOT_DIR)/../wpdk/include -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS'
export CC CXX CFLAGS CXXFLAGS LD CCAR

./configure --with-dpdk=C:/Users/Nick/Repos/dpdk/build --enable-debug --without-isal --without-vhost  --without-virtio
