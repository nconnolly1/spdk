#!/usr/bin/env bash

PATH=/c/Progra~1/LLVM/bin:/c/Progra~1/NASM:/c/Progra~1/meson:/c/Progra~2/Micros~2/2019/Enterprise/VC/Tools/MSVC/14.27.29110/bin/Hostx64/x64:$PATH
export PATH

CC=clang
CXX=clang
LD=lld-link
CCAR=lld-link
CFLAGS='-I$(SPDK_ROOT_DIR)/include/windows -I$(SPDK_ROOT_DIR)/../CUnit/include -I$(SPDK_ROOT_DIR)/../wpdk/include -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_CRT_DECLARE_NONSTDC_NAMES=0'
CXXFLAGS='-I$(SPDK_ROOT_DIR)/include/windows -I$(SPDK_ROOT_DIR)/../CUnit/include -I$(SPDK_ROOT_DIR)/../wpdk/include -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS -D_CRT_DECLARE_NONSTDC_NAMES=0'
LDFLAGS='-L$(SPDK_ROOT_DIR)/../CUnit/lib -L$(SPDK_ROOT_DIR)/../wpdk/lib'
export CC CXX CFLAGS CXXFLAGS LD LDFLAGS CCAR

./configure --with-dpdk=C:/Users/Nick/Repos/dpdk/build --enable-debug --without-isal --without-vhost  --without-virtio