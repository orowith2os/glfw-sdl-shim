project('glfw-sdl-shim', ['cpp', 'c'],
          version: '0.1.0',
    meson_version: '>= 1.0.0',
  default_options: [ 'warning_level=2', 'werror=false', 'cpp_std=c++23', ],
)

include_dir = include_directories('include')

sdl3_dep = dependency('SDL3')
gl_dep = dependency('gl')

subdir('src')