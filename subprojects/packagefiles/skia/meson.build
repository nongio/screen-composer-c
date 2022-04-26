project('skia', 'cpp',
  version: '0.0.0',
  meson_version: '>=0.58.1',
  default_options: [
    'cpp_std=c++17',
  ],
)
cc = meson.get_compiler('c')


message('sync skia git dependencies...\n(this is going to take some time)\n')

r = run_command('python3',  'tools/git-sync-deps', check:true)
message(r.stdout().strip())

message('building skia...\n(go grab a coffee)\n')
r = run_command('./build_skia.sh', check:true)
message(r.stdout().strip())


headers = [
  './',
  './include',
]

dep = declare_dependency(
  dependencies : cc.find_library('skia', dirs : meson.current_source_dir() / 'out' / 'Static'),
  include_directories : include_directories(headers))