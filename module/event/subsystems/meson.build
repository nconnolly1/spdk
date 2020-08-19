# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017-2019 Intel Corporation


# process all libraries equally, as far as possible
# "core" libs first, then others alphebetically as far as possible
# NOTE: for speed of meson runs, the dependencies in the subdirectories
# sometimes skip deps that would be implied by others, e.g. if mempool is
# given as a dep, no need to mention ring. This is especially true for the
# core libs which are widely reused, so their deps are kept to a minimum.
libraries = [
	'accel',
	'bdev',
###	'iscsi',
	'nbd',
	'nvmf',
	'scsi',
	'sock',
	'vhost',
	'vmd',
	]

# fix: blobfs, conf
if is_windows
  #libraries = ['bdev'] # only supported libraries for windows
endif

default_cflags = machine_args
default_cflags += ['-DALLOW_EXPERIMENTAL_API']
default_cflags += ['-DALLOW_INTERNAL_API']

# SPDK - WINDOWS
default_cflags += ['-Wno-unused-parameter']
default_cflags += ['-Wno-unused-function']
default_cflags += ['-Wno-pointer-arith']
default_cflags += ['-Wno-microsoft-enum-forward-reference']
default_cflags += ['-Wno-undef']
default_cflags += ['-Wno-cast-qual']
default_cflags += ['-Wno-format']
default_cflags += ['-Wno-incompatible-pointer-types-discards-qualifiers']
default_cflags += ['-Wno-deprecated-declarations']
default_cflags += ['-Wno-format-nonliteral']
default_cflags += ['-Wno-pointer-sign']
default_cflags += ['-Wno-int-to-void-pointer-cast']
default_cflags += ['-Wno-incompatible-pointer-types']
default_cflags += ['-D_CRT_SECURE_NO_WARNINGS']

if cc.has_argument('-Wno-format-truncation')
	default_cflags += '-Wno-format-truncation'
endif

enabled_libs = [] # used to print summary at the end

foreach l:libraries
	build = true
	reason = '<unknown reason>' # set if build == false to explain why
	name = l
	use_function_versioning = false
	sources = []
	headers = []
	includes = []
	cflags = default_cflags
	objs = [] # other object files to link against, used e.g. for
	          # instruction-set optimized versions of code

	# use "deps" for internal DPDK dependencies, and "ext_deps" for
	# external package/library requirements
	ext_deps = []
	deps = []
	# eal is standard dependency once built
	if dpdk_conf.has('RTE_LIBRTE_EAL')
		deps += ['eal']
	endif

	dir_name = l
	subdir(dir_name)

	if build
		shared_deps = ext_deps
		static_deps = ext_deps
		foreach d:deps
			if not is_variable('shared_spdk_' + d)
				error('Missing internal dependency "@0@" for @1@ [@2@]'
						.format(d, name, 'lib/' + dir_name))
			endif
			shared_deps += [get_variable('shared_spdk_' + d)]
			static_deps += [get_variable('static_spdk_' + d)]
		endforeach
	endif

	if not build
		dpdk_libs_disabled += name
		set_variable(name.underscorify() + '_disable_reason', reason)
	else
		enabled_libs += name
		dpdk_conf.set('RTE_LIBRTE_' + name.to_upper(), 1)
		install_headers(headers)

		libname = 'spdk_event_' + name
		includes += include_directories(dir_name)

		if sources.length() == 0
			# if no C files, just set a dependency on header path
			shared_dep = declare_dependency(include_directories: includes)
			static_dep = shared_dep
		else

			if use_function_versioning
				cflags += '-DRTE_USE_FUNCTION_VERSIONING'
			endif

#			version_map = '@0@/@1@/spdk_@2@.map'.format(
#					meson.current_source_dir(), dir_name, name)
#
#			is_stable = run_command(is_stable_cmd,
#					files(version_map)).returncode() == 0
#
#			if is_stable
				lib_version = abi_version
				so_version = stable_so_version
#			else
#				lib_version = experimental_abi_version
#				so_version = experimental_so_version
#			endif

			# first build static lib
			static_lib = static_library(libname,
					sources,
					objects: objs,
					c_args: cflags,
					dependencies: static_deps,
					include_directories: includes,
					install: true)
			static_dep = declare_dependency(link_with: static_lib,
					include_directories: includes,
					dependencies: static_deps)

			if not use_function_versioning
				# use pre-build objects to build shared lib
				sources = []
				objs += static_lib.extract_all_objects(recursive: false)
			else
				# for compat we need to rebuild with
				# RTE_BUILD_SHARED_LIB defined
				cflags += '-DRTE_BUILD_SHARED_LIB'
			endif
#			version_map = '@0@/@1@/spdk_@2@.map'.format(
#					meson.current_source_dir(), dir_name, name)
#			implib = dir_name + '.dll.a'
#
#			def_file = custom_target(name + '_def',
#				command: [map_to_def_cmd, '@INPUT@', '@OUTPUT@'],
#				input: version_map,
#				output: 'spdk_@0@_exports.def'.format(name))
#
#			if is_ms_linker
#				lk_args = ['-Wl,/def:' + def_file.full_path(),
#					'-Wl,/implib:lib\\' + implib]
#			else
#				lk_args = ['-Wl,--version-script=' + version_map]
#			endif
#
#			lk_deps = [version_map, def_file]
#			if not is_windows
#				# on unix systems check the output of the
#				# check-symbols.sh script, using it as a
#				# dependency of the .so build
#				lk_deps += custom_target(name + '.sym_chk',
#					command: [check_symbols,
#						version_map, '@INPUT@'],
#					capture: true,
#					input: static_lib,
#					output: name + '.sym_chk')
#			endif
#
#			shared_lib = shared_library(libname,
#					sources,
#					objects: objs,
#					c_args: cflags,
#					dependencies: shared_deps,
#					include_directories: includes,
#					link_args: lk_args,
#					link_depends: lk_deps,
#					version: lib_version,
#					soversion: so_version,
#					install: true)
#			shared_dep = declare_dependency(link_with: shared_lib,
#					include_directories: includes,
#					dependencies: shared_deps)
#
#			dpdk_libraries = [shared_lib] + dpdk_libraries
#			dpdk_static_libraries = [static_lib] + dpdk_static_libraries
#			if libname == 'rte_node'
#				dpdk_graph_nodes = [static_lib]
#			endif
		endif # sources.length() > 0

#		set_variable('shared_spdk_' + name, shared_dep)
		set_variable('static_spdk_' + name, static_dep)
		message('lib/@0@: Defining dependency "@1@"'.format(
				dir_name, name))
	endif # if build
endforeach
