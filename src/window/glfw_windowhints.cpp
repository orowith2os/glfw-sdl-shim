#include "helpers/glfw_helpers.hpp"
#include "glfw_window.hpp"

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_video.h>

#include <string_view>
#include <string>

void
glfwDefaultWindowHints ()
{
  ASSERT_MAIN_THREAD_VOID ()

  hints = WindowHints{};
}

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

void
glfwWindowHint (int hint, int value)
{
  ASSERT_MAIN_THREAD_VOID ()

  int err = GLFW_NO_ERROR;
  std::string_view ctx = "";

  switch (hint)
    {
    case GLFW_RESIZABLE:
      bool_flags_hint_helper (value, SDL_WINDOW_RESIZABLE, &err);
      break;
    case GLFW_VISIBLE:
      bool_flags_hint_helper_inverted (value, SDL_WINDOW_HIDDEN, &err);
      break;
    case GLFW_DECORATED:
      bool_flags_hint_helper_inverted (value, SDL_WINDOW_BORDERLESS, &err);
      break;
    case GLFW_FOCUSED:
    case GLFW_AUTO_ICONIFY:
      return; // global hint, not per-window, in SDL
    case GLFW_FLOATING:
      bool_flags_hint_helper (value, SDL_WINDOW_ALWAYS_ON_TOP, &err);
      break;
    case GLFW_MAXIMIZED:
      bool_flags_hint_helper (value, SDL_WINDOW_MAXIMIZED, &err);
      break;
    case GLFW_CENTER_CURSOR:
      return; // TODO: need to handle manually. Warp cursor to center.
    case GLFW_TRANSPARENT_FRAMEBUFFER:
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
        {
          err = GLFW_INVALID_VALUE;
          ctx = "Passed an unknown API for GLFW_CLIENT_API.";
        }
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
        {
          err = GLFW_INVALID_VALUE;
          ctx = "Unknown OpenGL profile passed for GLFW_OPENGL_PROFILE.";
        }
      break;
    case GLFW_OPENGL_FORWARD_COMPAT:
      bool_hint_helper (value, &hints.forward_compat, &err);
      break;
    case GLFW_OPENGL_DEBUG_CONTEXT:
      bool_hint_helper (value, &hints.debug_context, &err);
      break;

    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Unknown enum variant for SetWindowHint. File a bug.";
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
