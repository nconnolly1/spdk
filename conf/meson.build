# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017-2019 Intel Corporation

fs = import('fs')

# check the OS is supported, rather than going any further
supported_exec_envs = ['freebsd', 'linux', 'windows']
exec_env = host_machine.system()
if not supported_exec_envs.contains(exec_env)
	error('unsupported system type "@0@"'.format(exec_env))
endif

# define a handy variable for checking which OS we have.
# gives us "is_windows", "is_freebsd" and "is_linux"
foreach env:supported_exec_envs
	set_variable('is_' + env, exec_env == env)
endforeach

subdir('scripts')

# MS linker requires special treatment.
# TODO: use cc.get_linker_id() with Meson >= 0.54
is_ms_linker = is_windows and (cc.get_id() == 'clang')

# set the major version, which might be used by drivers and libraries
# depending on the configuration options
pver = meson.project_version().split('.')
major_version = '@0@.@1@'.format(pver.get(0), pver.get(1))

# currently no ABI guarantees
abi_version = meson.project_version()

# Libraries have the abi_version as the filename extension
# and have the soname be all but the final part of the abi_version.
# e.g. v20.1 => librte_foo.so.20.1
#    sonames => librte_foo.so.20
# e.g. v20.0.1 => librte_foo.so.20.0.1
#      sonames => librte_foo.so.20.0
abi_va = abi_version.split('.')
so_version = abi_va.length() == 2 ? abi_va[0] : abi_va[0] + '.' + abi_va[1]

# load the config file
default_config = run_command(find_program('cat', 'more'), config_file).stdout()
foreach line : default_config.split('\n')
    line = line.strip()
	if not (line == '') and not line.startswith('#')
		s = line.split('=')
		val = s[1].strip()
		if val.to_lower() == 'y'
			spdk_conf.set(s[0].strip(), true)
		elif val.to_lower() == 'n'
			spdk_conf.set(s[0].strip(), false)
		else
			spdk_conf.set(s[0].strip(), s[1].strip().strip('"'))
		endif
	endif
endforeach

# set the machine type and cflags for it
if meson.is_cross_build()
	machine = host_machine.cpu()
else
	machine = get_option('target-arch')
endif

# machine type 'default' matches the minimal baseline needed for DPDK.
if machine == 'default'
	if host_machine.cpu_family().startswith('x86')
		machine = 'corei7'
	elif host_machine.cpu_family().startswith('arm')
		machine = 'armv7-a'
	elif host_machine.cpu_family().startswith('aarch')
		# arm64 manages defaults in conf/arm/meson.build
		machine = 'default'
	elif host_machine.cpu_family().startswith('ppc')
		machine = 'power8'
	endif
endif

spdk_conf.set('CONFIG_ARCH', machine)

#@  		--cross-prefix=*)
#@  			CONFIG[CROSS_PREFIX]="${i#*=}"
# HACK - need to figure out cross compilation properly - is there a prefix?
# HACK host_machine to determine something ???

if not meson.is_cross_build()
	spdk_conf.set('CONFIG_CROSS_PREFIX', '')
endif

# Sanitize default configuration. All parameters set by user explicit should fail
arch = host_machine.cpu_family()

# Force no ISA-L if non-x86 or non-aarch64 architecture
if spdk_conf.get('CONFIG_ISAL', false)
	if not (arch.startswith('x86_x64') or arch.startswith('aarch64'))
		spdk_conf.set('CONFIG_ISAL', false)
	endif
endif

if not is_linux
	# Vhost, rte_vhost library and virtio are only supported on Linux.
	spdk_conf.set('CONFIG_VHOST', false)
	spdk_conf.set('CONFIG_VHOST_INTERNAL_LIB', false)
	spdk_conf.set('CONFIG_VIRTIO', false)
	message('Notice: Vhost, rte_vhost library and virtio are only supported on Linux. Turning off default feature.')
endif

#check nasm only on x86
if arch.startswith('x86_64')
	nasm = find_program('nasm', required: false, version: '>=2.13.03')
	if not nasm.found()
		spdk_conf.set('CONFIG_ISAL', false)
		spdk_conf.set('CONFIG_CRYPTO', false)
		spdk_conf.set('CONFIG_IPSEC_MB', false)
		spdk_conf.set('CONFIG_REDUCE', false)
		message('Notice: ISA-L, compression & crypto require NASM version 2.13.03 or newer. Turning off default ISA-L and crypto features.')
	endif
endif

## HACk - decide cross-prefix handling
#@  		--cross-prefix=*) ;&

if get_option('prefix') != ''
	spdk_conf.set('CONFIG_PREFIX', get_option('prefix'))
endif

spdk_conf.set('CONFIG_DEBUG', get_option('debug'))
spdk_conf.set('CONFIG_WERROR', get_option('werror'))
spdk_conf.set('CONFIG_LTO', get_option('b_lto'))
spdk_conf.set('CONFIG_COVERAGE', get_option('b_coverage'))
spdk_conf.set('CONFIG_TSAN', (get_option('b_sanitize') == 'thread'))
spdk_conf.set('CONFIG_ASAN', (get_option('b_sanitize') == 'address'))
spdk_conf.set('CONFIG_UBSAN', (get_option('b_sanitize') == 'undefined'))
spdk_conf.set('CONFIG_PGO_CAPTURE', (get_option('b_pgo') == 'generate'))
spdk_conf.set('CONFIG_PGO_USE', (get_option('b_pgo') == 'use'))
spdk_conf.set('CONFIG_SHARED', (get_option('default_library') != 'static'))

# process enable and disable options
foreach opt : ['tests', 'unit-tests', 'examples']
	if get_option('disable-' + opt)
		if opt in get_option('enable-' + opt)
			error('Conflicting options: "' + opt + '" is enabled and disabled.')
		endif
		spdk_conf.set('CONFIG_' + opt.to_upper().underscorify(), false)
	elif get_option('enable-' + opt)
		spdk_conf.set('CONFIG_' + opt.to_upper().underscorify(), true)
	endif
endforeach

# process simple dependencies
foreach opt : ['rbd', 'iscsi-initiator', 'crypto', 'vhost', 'virtio', 'reduce',
		'igb-uio-driver', 'isal', 'fuse', 'nvme-cuse', 'raid5', 'idxd', 'internal-vhost-lib']
	var = 'CONFIG_' + (opt != 'internal-vhost-lib' ? opt.to_upper().underscorify() : 'VHOST_INTERNAL_LIB')
	if get_option('without-' + opt)
		if get_option('with-' + opt)
			error('Conflicting options: "' + opt + '" is with and without.')
		endif
		spdk_conf.set(var, false)
	elif get_option('with-' + opt)
		spdk_conf.set(var, true)
	endif
endforeach

# process dependencies that can specify path
dir_list = ['dpdk', 'pmdk', 'vpp', 'vtune']
path_list = ['uring', 'fc', 'ocf']
path_required = ['dpdk', 'vtune']

foreach opt : dir_list + path_list
	var = 'CONFIG_' + opt.to_upper().underscorify()
	dir_var = var + opt in dir_list ? '_DIR' : '_PATH'
	if get_option('without-' + opt)
		if get_option('with-' + opt) != 'undefined'
			error('Conflicting options: "' + opt + '" is with and without.')
		endif
		if opt != 'dpdk'
			spdk_conf.set(var, false)
		endif
		spdk_conf.set(dir_var, '')
	elif get_option('with-' + opt) != 'undefined'
		dir = get_option('with-' + opt)
		if dir != ''
			dir = join_paths(meson.source_root(), dir)
			if opt in dir_list and not fs.is_dir(dir)
				error(dir + ': directory not found')
			endif
		elif opt in path_required
			error('Path is required for dependency "' + opt + '"')
		endif
		if opt != 'dpdk'
			spdk_conf.set(var, true)
		endif
		spdk_conf.set(dir_var, dir)
	endif
endforeach

# process rdma
if get_option('without-rdma')
	if get_option('with-rdma') != 'undefined'
		error('Conflicting options: "rdma" is with and without.')
	endif
	spdk_conf.set('CONFIG_RDMA', false)
	spdk_conf.set('CONFIG_RDMA_PROV', '')
elif get_option('with-rdma') != 'undefined'
	val = get_option('with-rdma')
	spdk_conf.set('CONFIG_RDMA', true)
	spdk_conf.set('CONFIG_RDMA_PROV', val != '' ? val : 'verbs')
endif

# process fio
if get_option('without-fio')
	if get_option('with-fio') != 'undefined'
		error('Conflicting options: "fio" is with and without.')
	endif
	spdk_conf.set('CONFIG_FIO_PLUGIN', false)
elif get_option('with-fio') != 'undefined'
	val = get_option('with-fio')
	if val != ''
		dir = join_paths(meson.source_root(), dir)
		if not fs.is_dir(dir)
			error(dir + ': directory not found')
		endif
		spdk_conf.set('CONFIG_FIO_SOURCE_DIR', val)
	endif
	spdk_conf.set('CONFIG_FIO_PLUGIN', true)
endif

# process with-env
if get_option('with-env') != 'undefined'
	val = get_option('with-env')
	if val == ''
		error('Path is required for dependency "env"')
	endif
	spdk_conf.set('CONFIG_ENV', val)
endif

#@  if [[ $arch == x86_64* ]]; then
#@  	BUILD_CMD=($CC -o /dev/null -x c $CPPFLAGS $CFLAGS $LDFLAGS -march=native)
## HACK - is the -march=native needed ?
## Add in cflags etc??

build_args = []
if not is_windows
	build_args += ['-I/usr/local/include', '-L/usr/local/lib']
endif

# IDXD uses Intel specific instructions.
# HACK - problematic for cross builds - add not meson.is_cross_build()
if spdk_conf.get('CONFIG_IDXD', false)
	vendor = run_command(get_cpu_vendor).stdout().strip()
	if not vendor.contains('GenuineIntel')
		error('IDXD cannot be used due to CPU incompatiblity.')
	endif
endif

# Detect architecture and force no ISA-L if non-x86 or non-aarch64 architecture
if spdk_conf.get('CONFIG_ISAL', false)
	if not (arch.startswith('x86_x64') or arch.startswith('aarch64'))
		error('ISA-L cannot be used due to CPU incompatiblity.')
	endif
endif

if not spdk_conf.get('CONFIG_ISAL', false)
	if spdk_conf.get('CONFIG_REDUCE', false)
		# HACK - change option wording 
		error('Conflicting options: --with-reduce is not compatible with --without-isal.')
	endif
endif

if spdk_conf.get('CONFIG_ENV', '') == ''
	spdk_conf.set('CONFIG_ENV', join_paths(meson.source_root(), 'lib/env_dpdk'))
	message('Using default SPDK env in ' + spdk_conf.get('CONFIG_ENV', ''))

	if spdk_conf.get('CONFIG_DPDK_DIR', '') == ''
		if not fs.is_file(join_paths(meson.source_root(), 'dpdk/config/common_base'))
			## HACK - path handling
			message('DPDK not found; please specify --with-dpdk=<path> or run:')
			error('  git submodule update --init')
		else
			spdk_conf.set('CONFIG_DPDK_DIR', join_paths(meson.source_root(), 'dpdk/build'))
			message('Using default DPDK in ' + spdk_conf.get('CONFIG_DPDK_DIR', ''))
		endif

		if spdk_conf.get('CONFIG_VHOST', false) and not spdk_conf.get('CONFIG_VHOST_INTERNAL_LIB', false)
  			# We lookup "common_linux" file to check if DPDK version is >= 19.05.
			# "common_linux" is available since exactly DPDK 19.05 - it was renamed
			# from "common_linuxapp".
			if not fs.is_file(join_paths(meson.source_root(), 'dpdk/config/common_linux'))
				message('Notice: Using internal, legacy rte_vhost library due to DPDK version < 19.05')
				spdk_conf.set('CONFIG_VHOST_INTERNAL_LIB', true)
			endif
		endif
	else
		if spdk_conf.get('CONFIG_VHOST', false) and not spdk_conf.get('CONFIG_VHOST_INTERNAL_LIB', false)
			# DPDK must be already built, so we can simply try to use the new rte_vhost.
			# It has a number of internal dependencies though, so don't try to link the
			# program, just compile it
			if not cc.compiles('''
#include <rte_vhost.h>' \
int main(void) { return rte_vhost_extern_callback_register(0, NULL, NULL); }
''',				args: build_args + ['-Wno-deprecated-declarations', '-Werror',
					'-I' + join_paths(meson.source_root(), 'include')], name: 'dpdk check')
				message('Notice: DPDK\'s rte_vhost not found or version < 19.05, using internal, legacy rte_vhost library.')
				spdk_conf.set('CONFIG_VHOST_INTERNAL_LIB', true)
			endif
		endif
	endif
else
	if spdk_conf.get('CONFIG_DPDK_DIR', '') != ''
		## HACK - parms need changing
		error('--with-env and --with-dpdk are mutually exclusive.')
	endif

	if spdk_conf.get('CONFIG_VHOST', false)
		message('Vhost is only supported when using the default DPDK environment. Disabling it.')
	endif
	# Always disable vhost, but only print the error message if the user explicitly turned it on.
	spdk_conf.set('CONFIG_VHOST', false)

	if spdk_conf.get('CONFIG_VIRTIO', false)
		message('Virtio is only supported when using the default DPDK environment. Disabling it.')
	endif
	# Always disable virtio, but only print the error message if the user explicitly turned it on.
	spdk_conf.set('CONFIG_VIRTIO', false)
endif

if spdk_conf.get('CONFIG_VTUNE', false)
	if spdk_conf.get('CONFIG_VTUNE_DIR', '') == ''
		# HACK - option format
		error('When VTune is enabled, you must specify the VTune directory using --with-vtune=path')
	endif
endif

if spdk_conf.get('CONFIG_ASAN', false)
	if spdk_conf.get('CONFIG_TSAN', false)
		error('ASAN and TSAN cannot be enabled at the same time.')
	endif
endif

if is_freebsd
	# FreeBSD doesn't support all configurations
	if spdk_conf.get('CONFIG_COVERAGE', false)
		error('CONFIG_COVERAGE not available on FreeBSD')
	endif
endif

if not is_linux
	if spdk_conf.get('CONFIG_VHOST', false)
		error('Vhost is only supported on Linux.')
	endif

	if spdk_conf.get('CONFIG_VHOST_INTERNAL_LIB', false)
		error('Internal rte_vhost library is only supported on Linux.')
	endif

	if spdk_conf.get('CONFIG_VIRTIO', false)
		error('Virtio is only supported on Linux.')
	endif
endif

if spdk_conf.get('CONFIG_RDMA', false)
	prov = spdk_conf.get('CONFIG_RDMA_PROV', '')
	if prov != 'verbs' and prov != 'mlx5_dv'
		error('Invalid RDMA provider specified, must be "verbs" or "mlx5_dv"')
	endif

	if not cc.links('''
#include <infiniband/verbs.h>
#include <rdma/rdma_verbs.h>
int main(void) { return 0; }
''',		args: ['-libverbs', '-lrdmacm'], name: 'rdma check')
		error('rdma requires libverbs and librdmacm. Please install then re-run this script.')
	endif

	if cc.compiles('<infiniband/verbs.h>\nint main(void) { return !!IBV_WR_SEND_WITH_INV; }\n',
			args: build_args, name: 'rdma send check')
		spdk_conf.set('CONFIG_RDMA_SEND_WITH_INVAL', true)
	else
		spdk_conf.set('CONFIG_RDMA_SEND_WITH_INVAL', false)
		message('''
*******************************************************************************
WARNING: The Infiniband Verbs opcode Send With Invalidate is either not
supported or is not functional with the current version of libibverbs installed
on this system. Please upgrade to at least version 1.1.

Beginning with Linux kernel 4.14, the kernel NVMe-oF initiator leverages Send
With Invalidate RDMA operations to improve performance. Failing to use the
Send With Invalidate operation on the NVMe-oF target side results in full
functionality, but greatly reduced performance. The SPDK NVMe-oF target will
be unable to leverage that operation using the currently installed version
of libibverbs, so Linux kernel NVMe-oF initiators based on kernels greater
than or equal to 4.14 will see significantly reduced performance.
*******************************************************************************"
''')
	endif

	if cc.compiles('#include <rdma/rdma_cma.h>\nint main(void) { return !!RDMA_OPTION_ID_ACK_TIMEOUT; }\n',
			args: build_args, name: 'rdma ack check')
		spdk_conf.set('CONFIG_RDMA_SET_ACK_TIMEOUT', true)
	else
		spdk_conf.set('CONFIG_RDMA_SET_ACK_TIMEOUT', false)
		message('RDMA_OPTION_ID_ACK_TIMEOUT is not supported')
	endif

	if prov == 'mlx5_dv'
		if not cc.compiles('''
#include <spdk/stdinc.h>
#include <infiniband/mlx5dv.h>
#include <rdma/rdma_cma.h>
int main(void) { return rdma_establish(NULL) ||
'!!IBV_QP_INIT_ATTR_SEND_OPS_FLAGS || !!MLX5_OPCODE_RDMA_WRITE; }
''',			args: build_args + ['-lmlx5', '-I' + join_paths(meson.source_root(), 'include')],
				name: 'rdma mlx5_dv check')
			error('mlx5_dv provider is not supported')
		endif
	endif

	message('Using ' + prov + ' RDMA provider')
endif

if spdk_conf.get('CONFIG_FC', false)
	path = spdk_conf.get('CONFIG_FC_PATH', '')
	if path != '' and not fs.is_dir(path)
		error(path + ': directory not found')
	endif
endif

if spdk_conf.get('CONFIG_ISAL', false) or spdk_conf.get('CONFIG_CRYPTO', false)
	if arch.startswith('x86_64') and not nasm.found()
		message('ISA-L, compression & crypto require NASM version 2.13.03 or newer.')
		error('Please install or upgrade them re-run this script.')
	endif
	if spdk_conf.get('CONFIG_CRYPTO', false)
		spdk_conf.set('CONFIG_IPSEC_MB', true)
	endif
endif

if spdk_conf.get('CONFIG_ISAL', false)
	if not fs.is_file(join_paths(meson.source_root(), 'isa-l/autogen.sh'))
		message('ISA-L was not found; To install ISA-L run:')
		error('  git submodule update --init')
	endif
## HACK how to run shell commands!
#@  	cd $rootdir/isa-l
#@  	ISAL_LOG=$rootdir/isa-l/spdk-isal.log
#@  	echo -n "Configuring ISA-L (logfile: $ISAL_LOG)..."
#@  	./autogen.sh &> $ISAL_LOG
#@  	./configure CFLAGS="-fPIC -g -O2" --enable-shared=no >> $ISAL_LOG 2>&1
#@  	echo "done."
#@  	cd $rootdir
#@  fi
endif

if spdk_conf.get('CONFIG_PMDK', false)
	if not cc.links('#include <libpmemblk.h>\nint main(void) { return 0; }\n',
			args: build_args + ['-lpmemblk'], name: 'pmdkblk check')
		error('pmdk requires libpmemblk. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_REDUCE', false)
	if not cc.links('#include <libpmem.h>\nint main(void) { return 0; }\n',
			args: build_args + ['-lpmem'], name: 'pmdk check')
		error('reduce requires libpmem. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_VPP', false)
	vpp_dir = spdk_conf.get('CONFIG_VPP_DIR', '')
	vpp_args = build_args + ['-lvppinfra', '-lvsm', '-lvlibmemoryclient']
	if vpp_dir != ''
		vpp_args += '-L' + join_files(vpp_dir, '/lib')
		vpp_args += '-I' + join_files(vpp_dir, '/include')
	endif
	if not cc.links('#include <vnet/session/application_interface.h>\nint main(void) { return 0; }\n',
			args: vpp_args, name: 'vpp check')
		error('vpp requires installed vpp. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_NVME_CUSE', false)
	if not cc.links('#define FUSE_USE_VERSION 31\n#include <fuse3/cuse_lowlevel.h>\n#include <fuse3/fuse_lowlevel.h>\n#include <fuse3/fuse_opt.h>\nint main(void) { return 0; }\n',
			args: build_args + ['-lfuse3', '-D_FILE_OFFSET_BITS=64'], name: 'cuse check')
		error('cuse requires libfuse3. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_RBD', false)
	if not cc.links('#include <rbd/librbd.h>\n#include <rados/librados.h>\n',
			args: build_args + ['-lrados', '-lrbd'], name: 'rbd check')
		error('rbd requires librados and librbd. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_ISCSI_INITIATOR', false)
	if not cc.links('''
#include <iscsi/iscsi.h>
#include <iscsi/scsi-lowlevel.h>
#if LIBISCSI_API_VERSION < 20150621
#error
#endif
int main(void) { return 0; }
''',		args: build_args + ['-L/usr/lib64/iscsi', '-liscsi'], name: 'iscs check')
		message('iscsi-initiator requires libiscsi with LIBISCSI_API_VERSION >= 20150621.')
		error('Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_ASAN', false)
	if not cc.links('int main(void) { return 0; }\n',
			args: build_args + ['-fsanitize=address'], name: 'asan check')
		error('asan requires libasan. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_UBSAN', false)
	if not cc.links('int main(void) { return 0; }\n',
			args: build_args + ['-fsanitize=undefined'], name: 'ubsan check')
		error('ubsan requires libubsan. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_TSAN', false)
	if not cc.links('int main(void) { return 0; }\n',
			args: build_args + ['-fsanitize=thread'], name: 'tsan check')
		error('tsan requires libtsan. Please install then re-run this script.')
	endif
endif

if spdk_conf.get('CONFIG_OCF', false)
	# If OCF_PATH is a file, assume it is a library and use it to compile with
	path = spdk_conf.get('CONFIG_OCF_PATH', '')
	spdk_conf.set('CONFIG_CUSTOMOCF', (path != '' and fs.is_file(path)))
endif

if spdk_conf.get('CONFIG_PGO_CAPTURE', false) and spdk_conf.get('CONFIG_PGO_USE', false)
	# HACK command line arguments - also can't have both defined with meson
	error('--enable-pgo-capture and --enable-pgo-use are mutually exclusive.')
elif spdk_conf.get('CONFIG_PGO_USE', false)
	if cc.get_id() == 'clang'
		# For clang we need to run an extra step on gathered profiling data.
		## HACK is this doesn automatically by meson?		
		message('Generating suitable profile data')
		run_command(find_program('llvm-profdata'), 'merge',
				'-output=' + join_paths(meson.source_root(), 'build/pgo/default.profdata'),
				join_paths(meson.source_root(), 'build/pgo')).stdout()
		endif		
endif

if spdk_conf.get('CONFIG_URING', false)
	path = spdk_conf.get('CONFIG_URING_PATH', '')
	if path != '' and not fs.is_dir(path)
		error(path + ': directory not found')
	endif
endif

if spdk_conf.get('CONFIG_FUSE', false)
	if not fs.is_dir('/usr/include/fuse3') and not fs.is_dir('/usr/local/include/fuse3')
		error('fuse requires libfuse3. Please install then re-run this script.')
	endif
endif


###
###  Rest of config here - set all the standard includes, defines etc
###


#@  ifneq ($(V),1)
#@  Q ?= @
#@  endif
#@  S ?= $(notdir $(CURDIR))
#@  
#@  DESTDIR?=
#@  
#@  ifneq ($(prefix),)
#@  CONFIG_PREFIX=$(prefix)
#@  endif
#@  
#@  bindir?=$(CONFIG_PREFIX)/bin
#@  libdir?=$(CONFIG_PREFIX)/lib
#@  includedir?=$(CONFIG_PREFIX)/include
#@  
#@  ifeq ($(MAKECMDGOALS),)
#@  MAKECMDGOALS=$(.DEFAULT_GOAL)
#@  endif
#@  
#@  TARGET_TRIPLET := $(shell $(CC) -dumpmachine)
#@  TARGET_TRIPLET_WORDS := $(subst -, ,$(TARGET_TRIPLET))
#@  
#@  TARGET_ARCHITECTURE ?= $(CONFIG_ARCH)
#@  TARGET_MACHINE := $(firstword $(TARGET_TRIPLET_WORDS))
#@  

#+  machine_args = []
#+  
#+  # ppc64 does not support -march= at all, use -mcpu and -mtune for that
#+  if host_machine.cpu_family().startswith('ppc')
#+  	machine_args += '-mcpu=' + machine
#+  	machine_args += '-mtune=' + machine
#+  else
#+  	machine_args += '-march=' + machine
#+  endif
#+  
#+  toolchain = cc.get_id()
#+  dpdk_conf.set_quoted('RTE_TOOLCHAIN', toolchain)
#+  dpdk_conf.set('RTE_TOOLCHAIN_' + toolchain.to_upper(), 1)
#+  
#+  dpdk_conf.set('RTE_ARCH_64', cc.sizeof('void *') == 8)
#+  
#+  if not is_windows
#+  	add_project_link_arguments('-Wl,--no-as-needed', language: 'c')
#+  endif
#+  
#+  # use pthreads if available for the platform
#+  if not is_windows
#+  	add_project_link_arguments('-pthread', language: 'c')
#+  	dpdk_extra_ldflags += '-pthread'
#+  endif
#+  
#+  # on some OS, maths functions are in a separate library
#+  if cc.find_library('m', required : false).found()
#+  	# some libs depend on maths lib
#+  	add_project_link_arguments('-lm', language: 'c')
#+  	dpdk_extra_ldflags += '-lm'
#+  endif
#+  
#+  # for linux link against dl, for bsd execinfo
#+  if is_linux
#+  	link_lib = 'dl'
#+  elif is_freebsd
#+  	link_lib = 'execinfo'
#+  else
#+  	link_lib = ''
#+  endif
#+  
#+  # if link_lib is empty, do not add it to project properties
#+  if link_lib != ''
#+  	add_project_link_arguments('-l' + link_lib, language: 'c')
#+  	dpdk_extra_ldflags += '-l' + link_lib
#+  endif
#+  
#+  # check for libraries used in multiple places in DPDK
#+  has_libnuma = 0
#+  numa_dep = cc.find_library('numa', required: false)
#+  if numa_dep.found() and cc.has_header('numaif.h')
#+  	dpdk_conf.set10('RTE_HAS_LIBNUMA', true)
#+  	has_libnuma = 1
#+  	add_project_link_arguments('-lnuma', language: 'c')
#+  	dpdk_extra_ldflags += '-lnuma'
#+  endif
#+  
#+  # check for libbsd
#+  libbsd = dependency('libbsd', required: false)
#+  if libbsd.found()
#+  	dpdk_conf.set('RTE_USE_LIBBSD', 1)
#+  endif
#+  
#+  # check for pcap
#+  pcap_dep = dependency('pcap', required: false)
#+  if pcap_dep.found()
#+  	# pcap got a pkg-config file only in 1.9.0 and before that meson uses
#+  	# an internal pcap-config finder, which is not compatible with
#+  	# cross-compilation, so try to fallback to find_library
#+  	pcap_dep = cc.find_library('pcap', required: false)
#+  endif
#+  if pcap_dep.found() and cc.has_header('pcap.h', dependencies: pcap_dep)
#+  	dpdk_conf.set('RTE_PORT_PCAP', 1)
#+  	dpdk_extra_ldflags += '-lpcap'
#+  endif
#+  
#+  # for clang 32-bit compiles we need libatomic for 64-bit atomic ops
#+  if cc.get_id() == 'clang' and dpdk_conf.get('RTE_ARCH_64') == false
#+  	atomic_dep = cc.find_library('atomic', required: true)
#+  	add_project_link_arguments('-latomic', language: 'c')
#+  	dpdk_extra_ldflags += '-latomic'
#+  endif
#+  
#+  # add -include rte_config to cflags
#+  add_project_arguments('-include', 'rte_config.h', language: 'c')
#+  

# enable extra warnings and disable any unwanted warnings
warning_flags = [
	# -Wall is added by meson by default, so add -Wextra only
	'-Wextra',

	# additional warnings in alphabetical order
	'-Wformat',
	'-Wformat-security',
	'-Wmissing-declarations',
	'-Wno-pointer-sign',
	'-Wold-style-definition',
	'-Wstrict-prototypes',

	# globally disabled warnings
	'-Wno-strict-aliasing',
	'-Wno-missing-field-initializers',
	'-Wno-unused-parameter',
]

foreach arg: warning_flags
	if cc.has_argument(arg)
		add_project_arguments(arg, language: 'c')
	endif
endforeach

## HACK add -I$(SPDK_ROOT_DIR)/include

#+  # set other values pulled from the build options
#+  dpdk_conf.set('RTE_MAX_LCORE', get_option('max_lcores'))
#+  dpdk_conf.set('RTE_MAX_NUMA_NODES', get_option('max_numa_nodes'))
#+  dpdk_conf.set('RTE_MAX_ETHPORTS', get_option('max_ethports'))
#+  dpdk_conf.set('RTE_LIBEAL_USE_HPET', get_option('use_hpet'))
#+  dpdk_conf.set('RTE_ENABLE_TRACE_FP', get_option('enable_trace_fp'))
# values which have defaults which may be overridden
#+  dpdk_conf.set('RTE_MAX_VFIO_GROUPS', 64)
#+  dpdk_conf.set('RTE_DRIVER_MEMPOOL_BUCKET_SIZE_KB', 64)
#+  dpdk_conf.set('RTE_LIBRTE_DPAA2_USE_PHYS_IOVA', true)
#+  if dpdk_conf.get('RTE_ARCH_64')
#+  	dpdk_conf.set('RTE_MAX_MEM_MB', 524288)
#+  else # for 32-bit we need smaller reserved memory areas
#+  	dpdk_conf.set('RTE_MAX_MEM_MB', 2048)
#+  endif
#+   
#+  compile_time_cpuflags = []
#+  subdir(arch_subdir)
#+  dpdk_conf.set('RTE_COMPILE_TIME_CPUFLAGS', ','.join(compile_time_cpuflags))
#+  


#@  ifneq ($(filter powerpc%,$(TARGET_MACHINE)),)
#@  COMMON_CFLAGS += -mcpu=$(TARGET_ARCHITECTURE)
#@  else ifeq ($(TARGET_MACHINE),aarch64)
#@  ifeq ($(TARGET_ARCHITECTURE),native)
#@  COMMON_CFLAGS += -march=armv8-a+crc
#@  else
#@  COMMON_CFLAGS += -march=$(TARGET_ARCHITECTURE)
#@  endif
#@  COMMON_CFLAGS += -DPAGE_SIZE=$(shell getconf PAGESIZE)
#@  else
#@  COMMON_CFLAGS += -march=$(TARGET_ARCHITECTURE)
#@  endif
#@  
#@  ifeq ($(CONFIG_WERROR), y)
#@  COMMON_CFLAGS += -Werror
#@  endif
#@  

## HACK - not needed?
##if spdk_conf.get('CONFIG_WERROR', false)
##	add_project_arguments('-W', language: 'c')
##endif

#@  ifeq ($(CONFIG_LTO),y)
#@  COMMON_CFLAGS += -flto
#@  LDFLAGS += -flto
#@  endif
#@  
#@  ifeq ($(CONFIG_PGO_CAPTURE),y)
#@  COMMON_CFLAGS += -fprofile-generate=$(SPDK_ROOT_DIR)/build/pgo
#@  LDFLAGS += -fprofile-generate=$(SPDK_ROOT_DIR)/build/pgo
#@  endif
#@  
#@  ifeq ($(CONFIG_PGO_USE),y)
#@  COMMON_CFLAGS += -fprofile-use=$(SPDK_ROOT_DIR)/build/pgo
#@  LDFLAGS += -fprofile-use=$(SPDK_ROOT_DIR)/build/pgo
#@  endif

# specify -D_GNU_SOURCE unconditionally
add_project_arguments('-D_GNU_SOURCE', language: 'c')
 

#@  
#@  # Always build PIC code so that objects can be used in shared libs and position-independent executables
#@  COMMON_CFLAGS += -fPIC
#@  
#@  # Enable stack buffer overflow checking
#@  COMMON_CFLAGS += -fstack-protector
#@  
#@  # Prevent accidental multiple definitions of global variables
#@  COMMON_CFLAGS += -fno-common
#@  
#@  # Enable full RELRO - no lazy relocation (resolve everything at load time).
#@  # This allows the GOT to be made read-only early in the loading process.
#@  LDFLAGS += -Wl,-z,relro,-z,now
#@  
#@  # Make the stack non-executable.
#@  # This is the default in most environments, but it doesn't hurt to set it explicitly.
#@  LDFLAGS += -Wl,-z,noexecstack
#@  
#@  # Specify the linker to use
#@  ifneq ($(LD_TYPE),)
#@  LDFLAGS += -fuse-ld=$(LD_TYPE)
#@  endif
#@  
#@  ifeq ($(OS),FreeBSD)
#@  SYS_LIBS += -L/usr/local/lib
#@  COMMON_CFLAGS += -I/usr/local/include
#@  endif
#@  
#@  # Attach only if PMDK lib specified with configure
#@  ifneq ($(CONFIG_PMDK_DIR),)
#@  LIBS += -L$(CONFIG_PMDK_DIR)/src/nondebug
#@  COMMON_CFLAGS += -I$(CONFIG_PMDK_DIR)/src/include
#@  endif
#@  
#@  ifneq ($(CONFIG_VPP_DIR),)
#@  LIBS += -L$(CONFIG_VPP_DIR)/lib64
#@  COMMON_CFLAGS += -I$(CONFIG_VPP_DIR)/include
#@  endif
#@  
#@  ifeq ($(CONFIG_RDMA),y)
#@  SYS_LIBS += -libverbs -lrdmacm
#@  endif
#@  
#@  ifeq ($(CONFIG_URING),y)
#@  SYS_LIBS += -luring
#@  ifneq ($(strip $(CONFIG_URING_PATH)),)
#@  CFLAGS += -I$(CONFIG_URING_PATH)
#@  LDFLAGS += -L$(CONFIG_URING_PATH)
#@  endif
#@  endif
#@  
#@  IPSEC_MB_DIR=$(SPDK_ROOT_DIR)/intel-ipsec-mb
#@  
#@  ISAL_DIR=$(SPDK_ROOT_DIR)/isa-l
#@  ifeq ($(CONFIG_ISAL), y)
#@  SYS_LIBS += -L$(ISAL_DIR)/.libs -lisal
#@  COMMON_CFLAGS += -I$(ISAL_DIR)/..
#@  endif
#@  
#@  #Attach only if FreeBSD and RDMA is specified with configure
#@  ifeq ($(OS),FreeBSD)
#@  ifeq ($(CONFIG_RDMA),y)
#@  # Mellanox - MLX4 HBA Userspace Library
#@  ifneq ("$(wildcard /usr/lib/libmlx4.*)","")
#@  SYS_LIBS += -lmlx4
#@  endif
#@  # Mellanox - MLX5 HBA Userspace Library
#@  ifneq ("$(wildcard /usr/lib/libmlx5.*)","")
#@  SYS_LIBS += -lmlx5
#@  endif
#@  # Chelsio HBA Userspace Library
#@  ifneq ("$(wildcard /usr/lib/libcxgb4.*)","")
#@  SYS_LIBS += -lcxgb4
#@  endif
#@  endif
#@  endif
#@  
#@  ifeq ($(CONFIG_DEBUG), y)
#@  COMMON_CFLAGS += -DDEBUG -O0 -fno-omit-frame-pointer
#@  else
#@  COMMON_CFLAGS += -DNDEBUG -O2
#@  # Enable _FORTIFY_SOURCE checks - these only work when optimizations are enabled.
#@  COMMON_CFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2
#@  endif
#@  
#@  ifeq ($(CONFIG_COVERAGE), y)
#@  COMMON_CFLAGS += -fprofile-arcs -ftest-coverage
#@  LDFLAGS += -fprofile-arcs -ftest-coverage
#@  ifeq ($(OS),FreeBSD)
#@  LDFLAGS += --coverage
#@  endif
#@  endif
#@  
#@  include $(CONFIG_ENV)/env.mk
#@  
#@  ifeq ($(CONFIG_ASAN),y)
#@  COMMON_CFLAGS += -fsanitize=address
#@  LDFLAGS += -fsanitize=address
#@  endif
#@  
#@  ifeq ($(CONFIG_UBSAN),y)
#@  COMMON_CFLAGS += -fsanitize=undefined
#@  LDFLAGS += -fsanitize=undefined
#@  endif
#@  
#@  ifeq ($(CONFIG_TSAN),y)
#@  COMMON_CFLAGS += -fsanitize=thread
#@  LDFLAGS += -fsanitize=thread
#@  endif
#@  
#@  SPDK_GIT_COMMIT := $(shell git rev-parse --short HEAD 2>/dev/null)
#@  ifneq (, $(SPDK_GIT_COMMIT))
#@  COMMON_CFLAGS += -DSPDK_GIT_COMMIT=$(SPDK_GIT_COMMIT)
#@  endif
#@  
#@  COMMON_CFLAGS += -pthread
#@  LDFLAGS += -pthread
#@  
#@  CFLAGS   += $(COMMON_CFLAGS) -std=gnu99
#@  CXXFLAGS += $(COMMON_CFLAGS)
#@  
#@  SYS_LIBS += -lrt
#@  SYS_LIBS += -luuid
#@  SYS_LIBS += -lcrypto
#@  
#@  ifneq ($(CONFIG_NVME_CUSE)$(CONFIG_FUSE),nn)
#@  SYS_LIBS += -lfuse3
#@  endif
#@  
#@  DEPFLAGS = -MMD -MP -MF $*.d.tmp
#@  
#@  	$(CC) -o $@ $(DEPFLAGS) $(CFLAGS) -c $< && \
#@  	mv -f $*.d.tmp $*.d && touch -c $@
#@  
#@  	$(CXX) -o $@ $(DEPFLAGS) $(CXXFLAGS) -c $< && \
#@  	mv -f $*.d.tmp $*.d && touch -c $@
#@  
#@  	$(CC) -o $@ $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) $(ENV_LINKER_ARGS) $(SYS_LIBS)
#@  	$(CXX) -o $@ $(CPPFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) $(ENV_LINKER_ARGS) $(SYS_LIBS)



#+  # set the install path for the drivers
#+  dpdk_conf.set_quoted('RTE_EAL_PMD_PATH', eal_pmd_path)
#+  
#+  install_headers('rte_config.h', subdir: get_option('include_subdir_arch'))
#+  
#+  # enable VFIO only if it is linux OS
#+  dpdk_conf.set('RTE_EAL_VFIO', is_linux)

#+  # specify -D__BSD_VISIBLE for FreeBSD
#+  if is_freebsd
#+  	add_project_arguments('-D__BSD_VISIBLE', language: 'c')
#+  endif
#+  
#+  if is_windows
#+  	# VirtualAlloc2() is available since Windows 10 / Server 2016.
#+  	add_project_arguments('-D_WIN32_WINNT=0x0A00', language: 'c')
#+  
#+  	# Use MinGW-w64 stdio, because DPDK assumes ANSI-compliant formatting.
#+  	if cc.get_id() == 'gcc'
#+  		add_project_arguments('-D__USE_MINGW_ANSI_STDIO', language: 'c')
#+  	endif
#+  
#+  	# Contrary to docs, VirtualAlloc2() is exported by mincore.lib
#+  	# in Windows SDK, while MinGW exports it by advapi32.a.
#+  	if is_ms_linker
#+  		add_project_link_arguments('-lmincore', language: 'c')
#+  	endif
#+  
#+  	add_project_link_arguments('-ladvapi32', '-lsetupapi', language: 'c')
#+  	add_project_link_arguments('-ldbghelp', language: 'c')
#+  endif
#+  
#+  if get_option('b_lto')
#+  	if cc.has_argument('-ffat-lto-objects')
#+  		add_project_arguments('-ffat-lto-objects', language: 'c')
#+  	else
#+  		error('compiler does not support fat LTO objects - please turn LTO off')
#+  	endif
#+  	# workaround for gcc bug 81440
#+  	if cc.get_id() == 'gcc' and cc.version().version_compare('<8.0')
#+  		add_project_arguments('-Wno-lto-type-mismatch', language: 'c')
#+  		add_project_link_arguments('-Wno-lto-type-mismatch', language: 'c')
#+  	endif
#+  endif


##########################################################################


# create config files
spdk_conf_file = configure_file(output: 'spdk_conf',
	configuration: spdk_conf)

run_command(gen_config_files, config_file, spdk_conf_file,
		join_paths(meson.source_root(), 'include/spdk/config.h'),
		join_paths(meson.source_root(), 'test/common/build_config.sh'))

subdir('mk')

## HACK - add install for generated files?