#!/usr/bin/env python
# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2019 Intel Corporation

import sys

def main(version_file):
    body = open(version_file, 'r').readlines()

    for line in body:
        s = line.strip()
        if "#define SPDK_VERSION_MAJOR\t" in s:
            major = s.split("SPDK_VERSION_MAJOR")[1].strip()
        elif "#define SPDK_VERSION_MINOR\t" in s:
            minor = s.split("SPDK_VERSION_MINOR")[1].strip()
        elif "#define SPDK_VERSION_PATCH\t" in s:
            patch = s.split("SPDK_VERSION_PATCH")[1].strip()
        elif "#define SPDK_VERSION_SUFFIX\t" in s:
            suffix = s.split("SPDK_VERSION_SUFFIX")[1].strip()
            
    if len(minor) < 2:
        minor = '0' + minor

    print(major + '.' + minor + '.' + patch + suffix.strip('"'))
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
    

