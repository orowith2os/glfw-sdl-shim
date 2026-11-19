#pragma once

#include "helpers/glfw_helpers.hpp"
#include <SDL3/SDL_video.h>

void
glfw_window_dispatch_event (SDL_WindowEvent event);

extern "C" {

struct WindowHints
{
  SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE |
                          SDL_WINDOW_HIGH_PIXEL_DENSITY;

  int client_api = GLFW_OPENGL_API;
  int opengl_profile = GLFW_OPENGL_ANY_PROFILE;
  int opengl_version_major = 1;
  int opengl_version_minor = 0;
  bool forward_compat = false;
  bool debug_context = false;

  // GL and framebuffer BS
  int red_bits = 8;
  int green_bits = 8;
  int blue_bits = 8;
  int alpha_bits = 8;
  int depth_bits = 24;
  int stencil_bits = 8;
  int samples = 0;
  bool srgb_capable = false;
  bool doublebuffer = true;

  std::string wayland_app_id;
};

struct GLFWwindow {
  // A window and GL context are 1-1 in GLFW,
  // you can't do multiple.
  SDL_Window *sdl_window;
  SDL_GLContext gl_context;
  int client_api;
  bool should_close;
};

} // extern "C"

inline WindowHints hints;