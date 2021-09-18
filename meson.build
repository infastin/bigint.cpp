project('bigint', 'cpp',
  version: '1.0.1',
  default_options: [
	'buildtype=debugoptimized',
	'warning_level=1',
	'cpp_std=c++17',
  ],
  license: 'MIT'
)

cxx = meson.get_compiler('cpp')

cppflags = []

if cxx.get_id() == 'g++' or cxx.get_id() == 'clang'
  cppflags = [
	'-pipe',
	'-funsigned-char'
  ]
elif cxx.get_id() == 'msvc'
  cppflags = [
	'/DBIGINT_EXPORTS',
  ]
endif

add_project_arguments(cxx.get_supported_arguments(cppflags), language: 'cpp')

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('test')

pkg_config = import('pkgconfig')
pkg_config.generate(
  libraries: libbigint,
  version: '1.0',
  name: 'libbigint',
  description: 'Arbitrary-Precision Integer Arithmetic Library'
)
