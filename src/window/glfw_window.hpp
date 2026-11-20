#pragma once

#include "cursor/glfw_cursor.hpp"
#include <SDL3/SDL_video.h>
#include <string>

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

  bool focus_on_show = true;
  bool fullscreen_center_cursor_on_first_focus = true;

  std::string wayland_app_id;
};

struct GLFWwindow {
  // A window and GL context are 1-1 in GLFW,
  // you can't do multiple.
  SDL_Window *sdl_window = nullptr;
  SDL_GLContext gl_context = nullptr;

  // Creation flags
  int client_api = GLFW_OPENGL_API;
  SDL_WindowFlags flags = 0;
  bool fullscreen_center_cursor_on_first_focus = false;

  bool focus_on_show = false;
  bool should_close = false;
  bool minimized = false;
  bool maximized = false;
  bool lock_key_mods = false;
  GLFWcursor *cursor = nullptr;
  void *user_pointer = nullptr;
  int cursor_mode = GLFW_CURSOR_NORMAL;
  char keys[GLFW_KEY_LAST + 1]; // GLFW_PRESS, GLFW_RELEASE, or GAY_ORO_GLFW_STICK (3)
  bool sticky_keys;

  // Callback hell.
  GLFWscrollfun scroll_callback = nullptr;
  GLFWkeyfun key_callback = nullptr;
  GLFWwindowsizefun window_size_callback = nullptr;
  GLFWwindowcontentscalefun content_scale_callback = nullptr;
  GLFWwindowrefreshfun window_refresh_callback = nullptr;
  GLFWwindowposfun window_pos_callback = nullptr;
  GLFWwindowclosefun close_callback = nullptr;
  GLFWwindowmaximizefun maximized_callback = nullptr;
  GLFWwindowiconifyfun iconify_callback = nullptr;
  GLFWwindowfocusfun focus_callback = nullptr;
  GLFWmousebuttonfun mouse_button_callback = nullptr;
  GLFWframebuffersizefun framebuffer_size_callback = nullptr;
  GLFWcursorposfun cursor_pos_callback = nullptr;
  GLFWcursorenterfun cursor_enter_callback = nullptr;
  GLFWdropfun drop_callback = nullptr;
  GLFWcharmodsfun char_mods_callback = nullptr;
  GLFWcharfun char_callback = nullptr;

  ~GLFWwindow ()
    {
      auto old_window = SDL_GL_GetCurrentWindow ();
      auto old_context = SDL_GL_GetCurrentContext ();

      if (cursor)
        glfwDestroyCursor (cursor);

      if (gl_context)
        SDL_GL_DestroyContext (gl_context);

      if (sdl_window)
        SDL_DestroyWindow (sdl_window);

      if (old_window && old_context &&
          old_window != sdl_window &&
          old_context != gl_context)
        SDL_GL_MakeCurrent(old_window, old_context);
    }
};

inline WindowHints hints;

void
bool_hint_helper (int value, bool *place, int *err);