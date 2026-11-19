ver_parts = meson.project_version().split('.')

conf_data = configuration_data()
conf_data.set('SHIM_MAJOR_VERSION', ver_parts[0])
conf_data.set('SHIM_MINOR_VERSION', ver_parts[1])
conf_data.set('SHIM_PATCH_VERSION', ver_parts[2])
conf_data.set_quoted('SHIM_VERSION_STRING', meson.project_version())

# Update these if the supported GLFW version stuff changes
conf_data.set('GLFW_MAJOR_VERSION', '3')
conf_data.set('GLFW_MINOR_VERSION', '5')
conf_data.set('GLFW_PATCH_VERSION', '1')
conf_data.set_quoted('GLFW_VERSION_STRING', '3.5.1')

configure_file(
  input : 'config.h.in',
  output : 'config.h',
  configuration : conf_data
)

glfw_sdl_shim_sources = [
  'glfw3.cpp',
  'glfw_timer.cpp',
  'stubs/glfw_stubs.cpp',
  'opengl/glfw_opengl.cpp',
  'events/glfw_events.cpp',
  'events/glfw_dispatchers.cpp',
  'window/glfw_window.cpp',
  'window/glfw_windowhints.cpp',
  'vulkan/glfw_vulkan.cpp',
  'helpers/glfw_helpers.cpp',
  'cursor/glfw_cursor.cpp',
  'gaming/glfw_gaming.cpp',
  'main.cpp',
]

glfw_sdl_shim_deps = [
  sdl3_dep,
  gl_dep,
  vk_dep,
]

shared_library('glfw-sdl-shim', glfw_sdl_shim_sources,
  dependencies: glfw_sdl_shim_deps,
  include_directories : include_dir,
  install: true,
)

executable('glfw-sdl-shim', glfw_sdl_shim_sources,
  dependencies: glfw_sdl_shim_deps,
  include_directories : include_dir,
  install: true,
)

