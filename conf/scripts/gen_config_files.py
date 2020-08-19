#!/usr/bin/env python
# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2020 Intel Corporation

import os
import re
import sys

assign = re.compile(r'^\s*([a-zA-Z0-9_]+)\s*(\?)?=\s*([^#]*)')
define = re.compile(r'^\s*(#\s*[a-z]+)\s*([a-zA-Z0-9_]+)([ \t]*[^#\n\r]*)')

config_defs = {}
current_defs = {}


# Create config.h

def create_h_file(target):
    file = open(target, 'w')
    file.write('#ifndef SPDK_CONFIG_H\n')
    file.write('#define SPDK_CONFIG_H\n')

    for item in current_defs.items():
        if isinstance(item[1], str) or item[1] == 1:
            file.write('#define SPDK_{} {}\n'.format(item[0], item[1]))
        else:
            file.write('#undef SPDK_{}\n'.format(item[0]))

    file.write('#endif /* SPDK_CONFIG_H */\n')
    file.close()
 

# Create build_config.sh

def create_sh_file(target):
    file = open(target, 'w')

    for item in current_defs.items():
        if isinstance(item[1], str):
            file.write('{}={}\n'.format(item[0], item[1]))
        else:
            file.write('{}={}\n'.format(item[0], 'y' if item[1] == 1 else 'n'))

    file.close()
 

# Create config.mk

def create_mk_file(config, target):
    file = open(target, 'w')
    
    for line in config:
        m = assign.match(line.strip())
        if m:
            var = m.group(1).strip()
            val = m.group(3).strip()

            if var in current_defs:
                val = current_defs[var]

            if isinstance(val, str):
                file.write('{}?={}\n'.format(var, val))
            else:
               file.write('{}?={}\n'.format(var, 'y' if val == 1 else 'n'))
        else:
            file.write(line)

    file.close()


# Load config file

def load_config(config_file):
    config = open(config_file, 'r').readlines()

    for line in config:
        m = assign.match(line.strip())
        if m:
            var = m.group(1).strip()
            val = m.group(3).strip()
            if val.lower() == 'y' or val.lower() == 'n':
                config_defs[var] = 1 if val.lower() == 'y' else 0
            else:
                config_defs[var] = val

    return config


# Load current file

def load_current(current_file):
    current = open(current_file, 'r').readlines()

    def get_define_value(var, val):
        if var in config_defs:
            return val.strip() if isinstance(config_defs[var], str) else 1
        # Meson currently generates a space after the name for text definitions.
        # Use it to indicate the type for variables that are not in CONFIG.
        return val.strip() if val else 1

    for line in current:
        m = define.match(line)
        if m:
            type = m.group(1).strip()
            var = m.group(2).strip()
            if type == '#define':
                current_defs[var] = get_define_value(var, m.group(3))
            elif type == '#undef':
                current_defs[var] = 0


def main(config_file, current_file, target_files):
    config = load_config(config_file)
    load_current(current_file)

    for target in target_files:
        if target.endswith('.h'):
            create_h_file(target)

        elif target.endswith('.sh'):
            create_sh_file(target)

        elif target.endswith('.mk'):
            create_mk_file(config, target)

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1], sys.argv[2], sys.argv[3:]))
