project('iconfigp', ['cpp'],
  license:         'MIT',
  version:         '0.1.3',
  default_options: [
    'cpp_std=c++20',
    'warning_level=3'
  ]
)

subdir('iconfigp')


if get_option('examples')
  subdir('examples')
endif
summary('examples', get_option('examples'))


if get_option('tests')
  subdir('tests')
endif
