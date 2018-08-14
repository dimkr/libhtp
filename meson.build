project('libhtp', 'c', license: 'BSD')

cflags = ['-D_GNU_SOURCE', '-std=gnu99']

h_sources = [
	'htp/bstr.h', 'htp/bstr_builder.h', 'htp/htp.h', 'htp/htp_base64.h', 'htp/htp_config.h', 'htp/htp_connection_parser.h',
	'htp/htp_core.h', 'htp/htp_decompressors.h', 'htp/htp_hooks.h', 'htp/htp_list.h',
	'htp/htp_multipart.h', 'htp/htp_table.h', 'htp/htp_transaction.h',
	'htp/htp_urlencoded.h', 'htp/htp_utf8_decoder.h'
]

c_sources = [
	'htp/bstr.c', 'htp/bstr_builder.c', 'htp/htp_base64.c', 'htp/htp_config.c', 'htp/htp_connection.c', 'htp/htp_connection_parser.c',
	'htp/htp_content_handlers.c', 'htp/htp_cookies.c', 'htp/htp_decompressors.c', 'htp/htp_hooks.c', 'htp/htp_list.c', 'htp/htp_multipart.c', 'htp/htp_parsers.c',
	'htp/htp_php.c', 'htp/htp_request.c', 'htp/htp_request_apache_2_2.c', 'htp/htp_request_generic.c', 'htp/htp_request_parsers.c', 'htp/htp_response.c',
	'htp/htp_response_generic.c', 'htp/htp_table.c', 'htp/htp_transaction.c', 'htp/htp_transcoder.c', 'htp/htp_urlencoded.c', 'htp/htp_util.c', 'htp/htp_utf8_decoder.c',
	'htp/strlcpy.c', 'htp/strlcat.c'
]

if get_option('debug')
	cflags += ['-DHTP_DEBUG']
endif

if get_option('debug')
	cflags += ['-DHTP_DEBUG']
endif

if get_option('devmode')
	cflags += ['-Werror',  '-Wfatal-errors']
endif

if get_option('devmode')
	cflags += ['-Werror',  '-Wfatal-errors']

	args = [['-fstack-protector'], ['-Wstrict-overflow=1'], ['-D_FORTIFY_SOURCE=2'], ['-Wformat', '-Wformat-security']]
	foreach args:
		has = []
		foreach arg: args
			if cc.has_argument(arg)
				has += [arg]
			endif
		endforeach
		if has.length() == args.length()
			cflags += has
		endif
	endforeach
endif

cc = meson.get_compiler('c')

foreach pair: [
	['strlcpy', 'string.h'],
	['strlcat', 'string.h']
]
	if cc.has_function(pair[0], prefix: '#include <@0@>'.format(pair[1]))
		cflags += ['-DHAVE_@0@=1'.format(pair[0].to_upper())]
	endif
endforeach

if cc.has_function('iconv', prefix: '#include <iconv.h>')
	cflags += ['-DHAVE_ICONV=1']

	code = '''#include <iconv.h>
ICONV_CONST int a = 1;'''
	if not cc.compiles(code)
		cflags += ['-DICONV_CONST=']
	endif
endif

deps = []

zlib = dependency('zlib', required: false)
if zlib.found()
	deps += [zlib]
endif

conf = configuration_data()
libhtp_version = run_command('grep', '^PKG_VERSION', 'VERSION').stdout().strip().split('=')[1]
conf.set('PACKAGE_VERSION', libhtp_version)
htp_version_h = configure_file(input: 'htp/htp_version.h.in',
                               output: 'htp_version.h',
                               configuration : conf)

libhtp_includes = [include_directories('.')]
libhtp = library('htp',
                 c_sources,
                 c_args: cflags,
                 include_directories: libhtp_includes,
                 install: true,
                 dependencies: deps)
install_headers(h_sources, subdir: 'htp')
install_headers(htp_version_h, subdir: 'htp')

pkgconfig = import('pkgconfig')
pkgconfig.generate(libraries : [libhtp],
                   subdirs: ['htp'],
                   version : libhtp_version,
                   name : 'LibHTP',
                   filebase : 'htp',
                   description : 'A security-aware HTTP parser, designed for use in IDS/IPS and WAF products.')
