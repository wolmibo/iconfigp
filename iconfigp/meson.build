dependencies = []



# find format library

conf_data = configuration_data()

code = '''
#include <format>
static_assert(__cpp_lib_format >= 201907L);
'''

compiler = meson.get_compiler('cpp')

if not compiler.compiles(code, args: '-std=c++20')
  dependencies += dependency('fmt')
  format_library = 'fmt'
else
  format_library = 'std'
endif
summary('format library', format_library)




sources = [
  'src/serialize.cpp',
  'src/format.cpp'
]

headers = [
  'include/iconfigp/exception.hpp',
  'include/iconfigp/format.hpp',
  'include/iconfigp/group.hpp',
  'include/iconfigp/key-value.hpp',
  'include/iconfigp/located-string.hpp',
  'include/iconfigp/opt-ref.hpp',
  'include/iconfigp/parser.hpp',
  'include/iconfigp/reader.hpp',
  'include/iconfigp/section.hpp',
  'include/iconfigp/serialize.hpp',
  'include/iconfigp/value-parser.hpp',
]

include_directories = ['include']



install_project = not meson.is_subproject() or get_option('install_as_subproject')

if meson.is_subproject()
  summary('install', install_project)
endif



iconfigp = library(
  'iconfigp',
  sources,
  dependencies:        dependencies,
  include_directories: include_directories,
  install:             install_project
)

if install_project
  install_headers(headers, subdir: 'iconfigp')

  pkg = import('pkgconfig')
  pkg.generate(iconfigp)
endif



iconfigp_dep = declare_dependency(
  link_with:           iconfigp,
  dependencies:        dependencies,
  include_directories: include_directories
)
