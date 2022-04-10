project('skia', 'cpp',
  version: '0.0.0',
  meson_version: '>=0.58.1',
  default_options: [
    'cpp_std=c++17',
  ],
)
cc = meson.get_compiler('c')

headers = [
  './',
  './include',
]
dep = declare_dependency(
  dependencies : cc.find_library('skia', dirs : meson.current_source_dir() / 'out' / 'Static'),
  include_directories : include_directories(headers))