# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2020 Intel Corporation

# HACK - add install for generated files?

# create cc.mk
cc_mk_conf = configuration_data()
cc_mk_conf.set('CC', cc.get_id())
cc_mk_conf.set('CC_TYPE', cc.get_id())
cc_mk_conf.set('CXX', cpp.get_id())
cc_mk_conf.set('CXX_TYPE', cpp.get_id())
cc_mk_conf.set('LD', cc.get_linker_id())
cc_mk_conf.set('LD_TYPE',
		cc.get_linker_id().startswith('ld.') ?
		cc.get_linker_id().substring(3) : cc.get_linker_id())

if get_option('b_lto')
	cc_mk_conf.set('CCAR', (cc.get_id() == 'clang') ? 'llvm-ar' : 'gcc-ar')
else
	cc_mk_conf.set('CCAR', 'ar')
endif

cc_mk_conf.set('CROSS_PREFIX', spdk_conf.get('CONFIG_CROSS_PREFIX', ''))
cc_mk_conf_file = configure_file(input: 'cc.mk.in',
		output: 'cc.mk', configuration: cc_mk_conf)
run_command(copy_to_source, cc_mk_conf_file,
		join_paths(meson.source_root(), 'mk/cc.mk'))

# create cc.flags.mk
c_args = ''
foreach arg : get_option('c_args')
	c_args += arg + ' '
endforeach

cpp_args = ''
foreach arg : get_option('c_args')	## HACK cpp_args
	cpp_args += arg + ' '
endforeach

c_link_args = ''
foreach arg : get_option('c_link_args')
	c_link_args += arg + ' '
endforeach

cc_flags_mk_conf = configuration_data()
cc_flags_mk_conf.set('CFLAGS', c_args)
cc_flags_mk_conf.set('CXXFLAGS', cpp_args)
cc_flags_mk_conf.set('LDFLAGS', c_link_args)
cc_flags_mk_conf.set('DESTDIR', '')

cc_flags_mk_conf_file = configure_file(input: 'cc.flags.mk.in',
		output: 'cc.flags.mk', configuration: cc_flags_mk_conf)
run_command(copy_to_source, cc_flags_mk_conf_file,
		join_paths(meson.source_root(), 'mk/cc.flags.mk'))

# create config.mk
run_command(gen_config_files, config_file, spdk_conf_file,
		join_paths(meson.source_root(), 'mk/config.mk'))
