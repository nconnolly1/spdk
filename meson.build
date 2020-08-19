# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017-2019 Intel Corporation

project('SPDK', 'C', 'CPP',
	# Get version number from file.
	# Fallback to "more" for Windows compatibility.
	version: run_command(find_program('cat', 'more'),
		files('VERSION')).stdout().strip(),
	license: 'BSD',
	default_options: ['buildtype=release', 'default_library=static'],
	meson_version: '>= 0.47.1'
)

# set up some global vars for compiler, platform, configuration, etc.
cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')
dpdk_conf = configuration_data()
dpdk_libraries = []
dpdk_static_libraries = []
dpdk_graph_nodes = []
dpdk_driver_classes = []
dpdk_drivers = []
dpdk_extra_ldflags = []
dpdk_app_link_libraries = []
dpdk_libs_disabled = []
dpdk_drvs_disabled = []
abi_version_file = files('ABI_VERSION')

if host_machine.cpu_family().startswith('x86')
	arch_subdir = 'x86'
elif host_machine.cpu_family().startswith('arm') or host_machine.cpu_family().startswith('aarch')
	arch_subdir = 'arm'
elif host_machine.cpu_family().startswith('ppc')
	arch_subdir = 'ppc'
endif

# configure the build, and make sure configs here and in config folder are
# able to be included in any file. We also store a global array of include dirs
# for passing to pmdinfogen scripts
global_inc = include_directories('.', 'include', 'include/windows', 'dpdk/config',
	'dpdk/lib/librte_eal/include',
	'dpdk/lib/librte_eal/@0@/include'.format(host_machine.system()),
	'dpdk/lib/librte_eal/@0@/include'.format(arch_subdir),
	'dpdk/lib/librte_pci',
	'dpdk/drivers/bus/pci',
	'dpdk/lib/librte_mempool',
	'dpdk/lib/librte_ring',
	'dpdk/drivers/bus/vdev',
	'dpdk/lib/librte_cryptodev',
	'dpdk/lib/librte_mbuf',
	'dpdk/lib/librte_kvargs',
	'dpdk/lib/librte_compressdev',
)
subdir('dpdk/config')

# build libs and drivers
subdir('buildtools')

subdir('lib')
subdir('module/accel')
#subdir('module/bdev')
#subdir('module/blob')
#subdir('module/blobfs')
#subdir('module/sock')
#subdir('module/event')
#subdir('module/event/subsystems')

# write the build config
build_cfg = 'rte_build_config.h'
configure_file(output: build_cfg,
		configuration: dpdk_conf,
		install_dir: join_paths(get_option('includedir'),
				get_option('include_subdir_arch')))

# for static builds, include the drivers as libs and we need to "whole-archive"
# them.
dpdk_drivers = ['-Wl,--whole-archive'] + dpdk_drivers + ['-Wl,--no-whole-archive']

pkg = import('pkgconfig')
pkg_extra_cflags = ['-include', 'rte_config.h'] + machine_args
if is_freebsd
	pkg_extra_cflags += ['-D__BSD_VISIBLE']
endif
pkg.generate(name: meson.project_name(),
	filebase: 'lib' + meson.project_name().to_lower(),
	version: meson.project_version(),
	libraries: dpdk_libraries,
	libraries_private: dpdk_drivers + dpdk_static_libraries +
			['-Wl,-Bdynamic'] + dpdk_extra_ldflags,
	requires: libbsd, # apps using rte_string_fns.h may need this if enabled
	                  # if libbsd is not enabled, then this is blank
	description: '''The Data Plane Development Kit (DPDK).
Note that CFLAGS might contain an -march flag higher than typical baseline.
This is required for a number of static inline functions in the public headers.''',
	subdirs: [get_option('include_subdir_arch'), '.'],
	extra_cflags: pkg_extra_cflags
)

