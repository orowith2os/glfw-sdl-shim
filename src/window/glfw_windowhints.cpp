#include "helpers/glfw_helpers.hpp"
#include "glfw_window.hpp"

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_video.h>

#include <string_view>
#include <string>
#include <format>

static void
bool_flags_hint_helper_inverted (int value, int flag, int *err)
{
  switch (value)
    {
      case GLFW_TRUE:
        hints.flags &= ~flag;
        break;
      case GLFW_FALSE:
        hints.flags |= flag;
        break;
      default:
        *err = GLFW_INVALID_VALUE;
        break;
    }
}

static void
bool_flags_hint_helper (int value, int flag, int *err)
{
  switch (value)
    {
      case GLFW_TRUE:
        hints.flags |= flag;
        break;
      case GLFW_FALSE:
        hints.flags &= ~flag;
        break;
      default:
        *err = GLFW_INVALID_VALUE;
        break;
    }
}

static void
int_bits_hint_helper (int value, int *place, int *err)
{
  if (value < 0 && value != GLFW_DONT_CARE)
    *err = GLFW_INVALID_VALUE;
  else
    *place = value;
}

void
bool_hint_helper (int value, bool *place, int *err)
{
  switch (value)
    {
    case GLFW_TRUE:
      *place = true;
      break;
    case GLFW_FALSE:
      *place = false;
      break;
    default:
      if (err)
        *err = GLFW_INVALID_VALUE;
      break;
    }
}

extern "C" {

#define BOOL(val)\
  val ? GLFW_TRUE : GLFW_FALSE;

void
glfwSetWindowAttrib (GLFWwindow *window, int attrib, int value)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)
  if (value != GLFW_TRUE && value != GLFW_FALSE)
    return;

  bool enabled = (value == GLFW_TRUE);

  switch (attrib)
    {
    case GLFW_DECORATED:
      window->decorated = enabled;
      SDL_SetWindowBordered(window->sdl_window, enabled);
      break;
    case GLFW_RESIZABLE:
      window->resizable = enabled;
      SDL_SetWindowResizable (window->sdl_window, enabled);
      break;
    case GLFW_FLOATING:
      window->floating = enabled;
      SDL_SetWindowAlwaysOnTop (window->sdl_window, enabled);
      break;
    case GLFW_AUTO_ICONIFY:
      window->minimize_on_focus_lost = enabled;
      if (state.focused_window == window)
        SDL_SetHint (SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, enabled ? "1" : "0");
      break;
    case GLFW_FOCUS_ON_SHOW:
      window->focus_on_show = enabled;
      if (state.focused_window == window)
        SDL_SetHint (SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, enabled ? "1" : "0");
      break;
    default:
      std::string ctx = std::format ("Invalid attribute passed to glfwGetWindowAttrib: {}", attrib);
      do_error_things (GLFW_INVALID_ENUM, ctx);
    }
}

#define BOOL_RET(val)\
  return val ? GLFW_TRUE : GLFW_FALSE;

int
glfwGetWindowAttrib (GLFWwindow *window, int attrib)
{
  ASSERT_GLFW_INIT (0)
  ASSERT_MAIN_THREAD (0)
  ASSERT_VALID_WIN (window, 0)

  switch (attrib)
    {
    case GLFW_FOCUSED:
      return ((state.focused_window == window)) ? GLFW_TRUE : GLFW_FALSE;
    case GLFW_ICONIFIED:
      BOOL_RET (window->minimized)
    case GLFW_MAXIMIZED:
      BOOL_RET (window->maximized)
    case GLFW_HOVERED:
      BOOL_RET (window->hovered)
    case GLFW_VISIBLE:
      BOOL_RET (!window->hidden)
    case GLFW_RESIZABLE:
      BOOL_RET (window->resizable)
    case GLFW_DECORATED:
      BOOL_RET (window->decorated)
    case GLFW_AUTO_ICONIFY:
      BOOL_RET (window->minimize_on_focus_lost)
    case GLFW_FLOATING:
      BOOL_RET (window->floating)
    case GLFW_TRANSPARENT_FRAMEBUFFER:
      BOOL_RET (window->transparent)
    case GLFW_FOCUS_ON_SHOW:
      BOOL_RET (window->focus_on_show)

    case GLFW_CLIENT_API:
      return window->client_api;
    case GLFW_CONTEXT_CREATION_API:
      return GLFW_NATIVE_CONTEXT_API; // No other option here.
    case GLFW_CONTEXT_VERSION_MAJOR:
      return window->gl_version_major;
    case GLFW_CONTEXT_VERSION_MINOR:
      return window->gl_version_minor;
    case GLFW_CONTEXT_REVISION:
      return window->gl_version_rev;
    case GLFW_OPENGL_FORWARD_COMPAT:
      BOOL_RET (window->gl_forward_compat)
    case GLFW_OPENGL_DEBUG_CONTEXT:
      BOOL_RET (window->gl_debug_context)
    case GLFW_OPENGL_PROFILE:
      return window->gl_profile;
    case GLFW_CONTEXT_RELEASE_BEHAVIOR:
      return window->gl_context_release_behavior;
    case GLFW_CONTEXT_NO_ERROR:
      BOOL_RET (window->gl_context_no_error)
    case GLFW_CONTEXT_ROBUSTNESS:
      return window->gl_context_robustness;

    default:
      std::string ctx = std::format ("Invalid attribute passed to glfwGetWindowAttrib: {}", attrib);
      do_error_things (GLFW_INVALID_ENUM, ctx);
    }

  return 0;
}

void
glfwDefaultWindowHints ()
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()

  hints = WindowHints{};
}

void
glfwWindowHint (int hint, int value)
{
  ASSERT_MAIN_THREAD_VOID ()

  int err = GLFW_NO_ERROR;
  std::string_view ctx = "";

  switch (hint)
    {
    case GLFW_RESIZABLE:
      bool_hint_helper (value, &hints.resizable, &err);
      bool_flags_hint_helper (value, SDL_WINDOW_RESIZABLE, &err);
      break;
    case GLFW_VISIBLE:
      bool_flags_hint_helper_inverted (value, SDL_WINDOW_HIDDEN, &err);
      break;
    case GLFW_DECORATED:
      bool_hint_helper (value, &hints.decorated, &err);
      bool_flags_hint_helper_inverted (value, SDL_WINDOW_BORDERLESS, &err);
      break;
    case GLFW_FOCUSED:

    case GLFW_AUTO_ICONIFY:
      bool_hint_helper (value, &hints.minimize_on_focus_lost, &err);
      return;
    case GLFW_FLOATING:
      bool_hint_helper (value, &hints.floating, &err);
      bool_flags_hint_helper (value, SDL_WINDOW_ALWAYS_ON_TOP, &err);
      break;
    case GLFW_MAXIMIZED:
      bool_flags_hint_helper (value, SDL_WINDOW_MAXIMIZED, &err);
      break;
    case GLFW_CENTER_CURSOR:
      bool_hint_helper (value, &hints.fullscreen_center_cursor_on_first_focus, &err);
      return;
    case GLFW_TRANSPARENT_FRAMEBUFFER:
      bool_hint_helper (value, &hints.transparent, &err);
      bool_flags_hint_helper (value, SDL_WINDOW_TRANSPARENT, &err);
      break;
    case GLFW_FOCUS_ON_SHOW:
      bool_hint_helper (value, &hints.focus_on_show, &err);
      break;
    case GLFW_SCALE_TO_MONITOR:
      return; // Not supported/TODO?
    case GLFW_SCALE_FRAMEBUFFER:
      bool_flags_hint_helper (value, SDL_WINDOW_HIGH_PIXEL_DENSITY, &err);
      break;
    case GLFW_MOUSE_PASSTHROUGH:
      return; // TODO? SDL_SetWindowMousePassthrough
    case GLFW_POSITION_X:
    case GLFW_POSITION_Y:
      return; // Not supported under Wayland. Don't bother.

    // GL BS
    case GLFW_RED_BITS:
      int_bits_hint_helper (value, &hints.red_bits, &err);
      break;
    case GLFW_GREEN_BITS:
      int_bits_hint_helper (value, &hints.green_bits, &err);
      break;
    case GLFW_BLUE_BITS:
      int_bits_hint_helper (value, &hints.blue_bits, &err);
      break;
    case GLFW_ALPHA_BITS:
      int_bits_hint_helper (value, &hints.alpha_bits, &err);
      break;
    case GLFW_DEPTH_BITS:
      int_bits_hint_helper (value, &hints.depth_bits, &err);
      break;
    case GLFW_STENCIL_BITS:
      int_bits_hint_helper (value, &hints.stencil_bits, &err);
      break;
    case GLFW_SAMPLES:
      int_bits_hint_helper (value, &hints.samples, &err);
      break;
    case GLFW_SRGB_CAPABLE:
      bool_hint_helper (value, &hints.srgb_capable, &err);
      break;
    case GLFW_DOUBLEBUFFER:
      bool_hint_helper (value, &hints.doublebuffer, &err);
      break;
    case GLFW_CLIENT_API:
      if (value == GLFW_OPENGL_API ||
          value == GLFW_OPENGL_ES_API ||
          value == GLFW_NO_API)
        hints.client_api = value;
      else
        err = GLFW_INVALID_VALUE;
      break;
    // Ignored, since SDL doesn't have an exact equivalent.
    // We just want platform-native everywhere. OSmesa was also
    // removed in Mesa as of 25.1.
    case GLFW_CONTEXT_CREATION_API:
      if (value != GLFW_NATIVE_CONTEXT_API &&
          value != GLFW_EGL_CONTEXT_API &&
          value != GLFW_OSMESA_CONTEXT_API)
        err = GLFW_INVALID_VALUE;
      break;
    case GLFW_CONTEXT_VERSION_MAJOR:
      if (value < 0)
        {
          err = GLFW_INVALID_VALUE;
          ctx = "Passed a negative value for GLFW_CONTEXT_VERSION_MAJOR";
        }
      hints.opengl_version_major = value;
      break;
    case GLFW_CONTEXT_VERSION_MINOR:
      if (value < 0)
        {
          err = GLFW_INVALID_VALUE;
          ctx = "Passed a negative value for GLFW_CONTEXT_VERSION_MINOR.";
        }
      else
        hints.opengl_version_minor = value;
      break;
    case GLFW_OPENGL_PROFILE:
      if (value == GLFW_OPENGL_CORE_PROFILE)
        hints.opengl_profile = SDL_GL_CONTEXT_PROFILE_CORE;
      else if (value == GLFW_OPENGL_COMPAT_PROFILE)
        hints.opengl_profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
      else if (value == GLFW_OPENGL_ANY_PROFILE)
        hints.opengl_profile = 0;
      else
        err = GLFW_INVALID_VALUE;
      break;
    case GLFW_OPENGL_FORWARD_COMPAT:
      bool_hint_helper (value, &hints.forward_compat, &err);
      break;
    case GLFW_OPENGL_DEBUG_CONTEXT:
      bool_hint_helper (value, &hints.debug_context, &err);
      break;

    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Unknown enum variant for glfwWindowHint. File a bug, if it's supposed to be valid.";
    }

  if (err == GLFW_INVALID_VALUE && ctx.empty())
    ctx = "Invalid value for hint";

  do_error_things (err, ctx);
}

void
glfwWindowHintString (int hint, const char* value)
{
  ASSERT_MAIN_THREAD_VOID ();

  int err = GLFW_NO_ERROR;
  std::string_view ctx = "";

  if (!value)
    {
      err = GLFW_INVALID_VALUE;
      ctx = "Window hint string value cannot be NULL";
      do_error_things (err, ctx);
      return;
    }

  switch (hint)
    {
    case GLFW_COCOA_FRAME_NAME:
      return; // macOS isn't supported (yet)
    case GLFW_WAYLAND_APP_ID:
      return; // SDL doesn't expose a way to set the appid per-window
    case GLFW_X11_CLASS_NAME:
    case GLFW_X11_INSTANCE_NAME:
      return; // X11 isn't supported (yet)
    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Invalid string window hint";
    }

  if (err == GLFW_INVALID_VALUE && ctx.empty())
    ctx = "Invalid value for hint";

  if (err != GLFW_NO_ERROR)
    do_error_things (err, ctx);
}

int
glfwGetInputMode (GLFWwindow *window, int mode)
{
  ASSERT_GLFW_INIT (GLFW_FALSE)
  ASSERT_MAIN_THREAD (GLFW_FALSE)
  if (!window)
    return GLFW_FALSE;

  int err = GLFW_NO_ERROR;
  std::string_view ctx = "";

  switch (mode)
    {
    case GLFW_CURSOR:
      return window->cursor_mode;
    case GLFW_STICKY_KEYS:
    case GLFW_STICKY_MOUSE_BUTTONS:
    case GLFW_LOCK_KEY_MODS:
    case GLFW_RAW_MOUSE_MOTION:
    case GLFW_UNLIMITED_MOUSE_BUTTONS:
      return GLFW_FALSE; // TODO?
    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Unknown enum variant for glfwSetInputMode. File a bug.";
    }

  do_error_things (err, ctx);
  return err;
}

void
glfwSetInputMode (GLFWwindow *window, int mode, int value)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window)
    return;

  int err = GLFW_NO_ERROR;
  std::string_view ctx = "";

  switch (mode)
    {
    case GLFW_CURSOR:
      switch (value)
        {
        case GLFW_CURSOR_NORMAL:
        case GLFW_CURSOR_HIDDEN:
        case GLFW_CURSOR_DISABLED:
        case GLFW_CURSOR_CAPTURED:
          window->cursor_mode = value;
          break;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
      if (err != GLFW_INVALID_VALUE && window->sdl_window)
        do_thing_with_cursor_mode (window);
      break;
    case GLFW_STICKY_KEYS:
      bool_hint_helper (value, &window->sticky_keys, &err);
      break;
    case GLFW_STICKY_MOUSE_BUTTONS:
      bool_hint_helper (value, &window->sticky_mouse, &err);
      break;
    case GLFW_LOCK_KEY_MODS:
      bool_hint_helper (value, &window->lock_key_mods, &err);
      break;
    case GLFW_UNLIMITED_MOUSE_BUTTONS:
    case GLFW_RAW_MOUSE_MOTION:
      return; // TODO?
    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Unknown enum variant for glfwSetInputMode. File a bug.";
    }

  if (err == GLFW_INVALID_VALUE && ctx.empty())
    ctx = "Invalid value for hint";

  do_error_things (err, ctx);
}

void
glfwSetWindowSizeLimits (GLFWwindow *window,
                         int minwidth, int minheight,
                         int maxwidth, int maxheight)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window)
    return;

  if ((minwidth != GLFW_DONT_CARE && minwidth < 0) ||
      (minheight != GLFW_DONT_CARE && minheight < 0) ||
      (maxwidth != GLFW_DONT_CARE && maxwidth < 0) ||
      (maxheight != GLFW_DONT_CARE && maxheight < 0))
    {
      std::string err = std::format ("Passed invalid size to glfwSetWindowSizeLimits: "
                                     "minwidth {}, minheight {}, maxwidth {}, maxheight {}",
                                     minwidth, minheight, maxwidth, maxheight);
      do_error_things (GLFW_INVALID_VALUE, err);
      return;
    }

  if ((minwidth != GLFW_DONT_CARE &&
       maxwidth != GLFW_DONT_CARE &&
       maxwidth < minwidth) ||
      (minheight != GLFW_DONT_CARE &&
       maxheight != GLFW_DONT_CARE &&
       maxheight < minheight))
    {
      if (maxwidth < minwidth)
        do_error_things (GLFW_INVALID_VALUE, "glfwSetWindowSizeLimits: maxwidth was smaller than minwidth");
      if (maxheight < minheight)
        do_error_things (GLFW_INVALID_VALUE, "glfwSetWindowSizeLimits: maxheight was smaller than maxheight");
      return;
    }

  // no-op
  SDL_WindowFlags flags = SDL_GetWindowFlags(window->sdl_window);
  if (!(flags & SDL_WINDOW_RESIZABLE))
    return;

  // map GLFW_DONT_CARE (-1) to SDL3's 0 (unlimited)
  int sdl_min_w = (minwidth == GLFW_DONT_CARE) ? 0 : minwidth;
  int sdl_min_h = (minheight == GLFW_DONT_CARE) ? 0 : minheight;
  int sdl_max_w = (maxwidth == GLFW_DONT_CARE) ? 0 : maxwidth;
  int sdl_max_h = (maxheight == GLFW_DONT_CARE) ? 0 : maxheight;

  if (!SDL_SetWindowMinimumSize (window->sdl_window, sdl_min_w, sdl_min_h))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
  if (!SDL_SetWindowMaximumSize (window->sdl_window, sdl_max_w, sdl_max_h))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}

}
