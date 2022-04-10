project('screen-composer', 'c', 'cpp',
  version: '0.0.0',
  license: 'MIT',
  meson_version: '>=0.58.1',
  default_options: [
    'c_std=c11',
    'cpp_std=c++17',
    'warning_level=2',
    'werror=true',
  ],
)

add_project_arguments(
  [
    '-DWLR_USE_UNSTABLE',
    '-Wundef',
    '-Wno-unused-parameter',
  ],
  language: ['c'],
)
add_project_arguments(
  [
    '-DWLR_USE_UNSTABLE',
    # '-Wundef',
    '-Wno-unused-parameter',
  ],
  language: ['cpp'],
)
if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language : 'c')
endif

cc = meson.get_compiler('c')

wlroots        = dependency('wlroots', version: '>= 0.15.0', fallback: ['wlroots', 'wlroots'])
wayland_protos = dependency('wayland-protocols', version: '>=1.14')
wayland_server = dependency('wayland-server')
xkbcommon      = dependency('xkbcommon')
pixman = dependency('pixman-1')
math           = cc.find_library('m')
glesv2 = dependency('glesv2')


wl_protocol_dir = wayland_protos.get_variable(pkgconfig:'pkgdatadir')
wayland_scanner = find_program('wayland-scanner')
wayland_scanner_server = generator(
  wayland_scanner,
  output: '@BASENAME@-protocol.h',
  arguments: ['server-header', '@INPUT@', '@OUTPUT@'],
)

server_protocols = [
  [wl_protocol_dir, 'stable/xdg-shell/xdg-shell.xml'],
  ['protocols/wlr-layer-shell-unstable-v1.xml'],
  ['protocols/sc-layer-unstable-v1.xml'],
]

wl_protos_src = []
wl_protos_headers = []

foreach p : server_protocols
	xml = join_paths(p)
	wl_protos_src += custom_target(
		xml.underscorify() + '_server_c',
		input: xml,
		output: '@BASENAME@-protocol.c',
		command: [wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@'],
	)
	wl_protos_headers += custom_target(
		xml.underscorify() + '_server_h',
		input: xml,
		output: '@BASENAME@-protocol.h',
		command: [wayland_scanner, 'server-header', '@INPUT@', '@OUTPUT@'],
	)
endforeach

lib_server_protos = static_library(
	'server_protos',
	wl_protos_src + wl_protos_headers,
	dependencies: wayland_server.partial_dependency(compile_args: true),
)

server_protos = declare_dependency(
	link_with: lib_server_protos,
	sources: wl_protos_headers,
)


version = '@0@'.format(meson.project_version())
git = find_program('git', native: true, required: false)
if git.found()
  git_commit = run_command([git, 'rev-parse', '--short', 'HEAD'])
  git_branch = run_command([git, 'rev-parse', '--abbrev-ref', 'HEAD'])
  if git_commit.returncode() == 0 and git_branch.returncode() == 0
    version = '@0@-@1@ (branch \'@2@\')'.format(
      meson.project_version(),
      git_commit.stdout().strip(),
      git_branch.stdout().strip(),
    )
  endif
endif

inih_dep = subproject('inih', default_options : []).get_variable('inih_dep')

conf_data = configuration_data()
conf_data.set_quoted('SC_VERSION', version)

sc_sources = [
  'src/main.c',
  'src/config.c',
  'src/compositor/compositor.c',
  'src/compositor/backend.c',
  'src/compositor/rendering.c',
  'src/compositor/cursor.c',
  'src/compositor/keyboard.c',
  'src/compositor/workspace.c',
  'src/compositor/xdg_shell.c',
  'src/compositor/wlr_layer_shell.c',
  'src/compositor/sc_layer_shell.c',
  'src/compositor/seat.c',
  'src/compositor/skia.cpp',
  'src/output/output.c',
  'src/output/repaintdelay.c',
  'src/output/view_iterators.c',
  'src/output/utils.c',
  'src/output/damage.c',
  'src/keyboard.c',
  'src/workspace.c',
  'src/view/view.c',
  'src/view/toplevel_view.c',
  'src/view/wlr_layer_view.c',
  'src/view/sc_layer_view.c',
  'src/view/popup_view.c',
  'src/gles2/renderer.c',
  'src/gles2/shader.c',
  'src/gles2/fbo.c',
  'src/utils/file.c',
  'src/layers-composer/shell.c',
  'src/layers-composer/layer.c',
  'src/layers-composer/animation.c',
]

sc_headers = [
  configure_file(input: 'config.h.in',
                 output: 'config.h',
                 configuration: conf_data),
]

sclib = library('sc',
  sc_sources,
  include_directories: include_directories('include'),
  dependencies: [
    server_protos,
    wayland_server,
    wlroots,
    xkbcommon,
    math,
	pixman,
    inih_dep,
    glesv2,
    skia,
    dependency('egl'),
    dependency('libjpeg'),
    dependency('libpng'),
    dependency('fontconfig'),
  ],
)

subdir('test')

executable(
  'scomposer',
  sc_sources,
  include_directories: include_directories('include'),
  dependencies: [
    server_protos,
    wayland_server,
    wlroots,
    xkbcommon,
    math,
	pixman,
    inih_dep,
    glesv2,
    skia,
    dependency('egl'),
    dependency('libjpeg'),
    dependency('libpng'),
    dependency('fontconfig'),
  ],
  install: true,
)

summary = [
  '',
  'ScreenComposer @0@'.format(version),
  '',
  ''
]
message('\n'.join(summary))
