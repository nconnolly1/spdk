#!/usr/bin/env bash

if [ $(uname -s) == "FreeBSD" ]; then
    sysctl -a | grep hw.model | cut -c 1-15 | sed -e's/Intel/GenuineIntel/'
else
    grep -i 'vendor' /proc/cpuinfo  --max-count=1
fi
