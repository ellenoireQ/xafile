project('xafile', 'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=2',
  ],
)

gnome = import('gnome')

gtk4_dep = dependency('gtk4', version: '>= 4.20.3')
adwaita_dep = dependency('libadwaita-1', version: '>= 1.8.2')

sources = files(
  'src/main.cpp',
  'src/application.cpp',
  'src/window.cpp',
  'src/sidebar.cpp',
  'src/content_view.cpp',
)

resources = gnome.compile_resources(
  'xafile-resources',
  'data/xafile.gresource.xml',
  source_dir: 'data',
)

executable('xafile',
  sources,
  resources,
  dependencies: [gtk4_dep, adwaita_dep],
  install: true,
)
