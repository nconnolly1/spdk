#!/usr/bin/env bash

CC=gcc.exe
CXX=gcc.exe
LD=ld.exe

export CC CXX LD

SKIP_DPDK_BUILD=1
export SKIP_DPDK_BUILD

bash ./configure --enable-debug --target-arch=nehalem --disable-tests --without-isal --without-vhost --without-iscsi-initiator --without-crypto --without-dpdk --disable-unit-tests

make
