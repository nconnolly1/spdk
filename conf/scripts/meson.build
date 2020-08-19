# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2020 Intel Corporation

# set up map-to-win script using python, either built-in or external
python3 = import('python').find_installation(required: false)
if python3.found()
	py3 = [python3]
else
	py3 = ['meson', 'runpython']
endif

gen_config_files = py3 + files('gen_config_files.py')
copy_to_source = py3 + files('copy_to_source.py')
map_to_win_cmd = py3 + files('map_to_win.py')

if build_machine.system()== 'windows'
	get_cpu_vendor = files('get_cpu_vendor.bat')
else
	get_cpu_vendor = files('get_cpu_vendor.sh')
endif