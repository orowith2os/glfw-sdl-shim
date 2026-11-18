#include "helpers/glfw_helpers.hpp"
#include "glfw_window.hpp"

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_video.h>

#include <string_view>
#include <string>

void
glfwDefaultWindowHints (void)
{
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
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags |= SDL_WINDOW_RESIZABLE;
            break;
          case GLFW_FALSE:
            hints.flags &= ~SDL_WINDOW_RESIZABLE;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }
      break;
    case GLFW_VISIBLE:
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags &= ~SDL_WINDOW_HIDDEN;
            break;
          case GLFW_FALSE:
            hints.flags |= SDL_WINDOW_HIDDEN;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }
      break;
    case GLFW_DECORATED:
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags &= ~SDL_WINDOW_BORDERLESS;
            break;
          case GLFW_FALSE:
            hints.flags |= SDL_WINDOW_BORDERLESS;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }
      break;
    case GLFW_FOCUSED:
    /*
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags |= SDL_WINDOW_INPUT_FOCUS;
            break;
          case GLFW_FALSE:
            hints.flags &= ~SDL_WINDOW_INPUT_FOCUS;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }*/ //TODO
      break;
    case GLFW_AUTO_ICONIFY:
      return; // global hint, not per-window, in SDL
    case GLFW_FLOATING:
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags |= SDL_WINDOW_ALWAYS_ON_TOP;
            break;
          case GLFW_FALSE:
            hints.flags &= ~SDL_WINDOW_ALWAYS_ON_TOP;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }
      break;
    case GLFW_MAXIMIZED:
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags |= SDL_WINDOW_MAXIMIZED;
            break;
          case GLFW_FALSE:
            hints.flags &= ~SDL_WINDOW_MAXIMIZED;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }
      break;
    case GLFW_CENTER_CURSOR:
      return; // TODO: need to handle manually. Warp cursor to center.
    case GLFW_TRANSPARENT_FRAMEBUFFER:
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags |= SDL_WINDOW_TRANSPARENT;
            break;
          case GLFW_FALSE:
            hints.flags &= ~SDL_WINDOW_TRANSPARENT;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }
      break;
    case GLFW_FOCUS_ON_SHOW:
      return; // Not supported per-window on SDL
    case GLFW_SCALE_TO_MONITOR:
      return; // Not supported/TODO?
    case GLFW_SCALE_FRAMEBUFFER:
      switch (value)
        {
          case GLFW_TRUE:
            hints.flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
            break;
          case GLFW_FALSE:
            hints.flags &= ~SDL_WINDOW_HIGH_PIXEL_DENSITY;
            break;
          default:
            err = GLFW_INVALID_VALUE;
            break;
        }
      break;
    case GLFW_MOUSE_PASSTHROUGH:
      return; // TODO? SDL_SetWindowMousePassthrough
    case GLFW_POSITION_X:
    case GLFW_POSITION_Y:
      return; // Not supported under Wayland. Don't bother.

    // GL BS
    case GLFW_RED_BITS:
      if (value < 0 && value != GLFW_DONT_CARE)
        err = GLFW_INVALID_VALUE;
      else
        hints.red_bits = value;
      break;
    case GLFW_GREEN_BITS:
      if (value < 0 && value != GLFW_DONT_CARE)
        err = GLFW_INVALID_VALUE;
      else
        hints.green_bits = value;
      break;
    case GLFW_BLUE_BITS:
      if (value < 0 && value != GLFW_DONT_CARE)
        err = GLFW_INVALID_VALUE;
      else
        hints.blue_bits = value;
      break;
    case GLFW_ALPHA_BITS:
      if (value < 0 && value != GLFW_DONT_CARE)
        err = GLFW_INVALID_VALUE;
      else
        hints.alpha_bits = value;
      break;
    case GLFW_DEPTH_BITS:
      if (value < 0 && value != GLFW_DONT_CARE)
        err = GLFW_INVALID_VALUE;
      else
        hints.depth_bits = value;
      break;
    case GLFW_STENCIL_BITS:
      if (value < 0 && value != GLFW_DONT_CARE)
        err = GLFW_INVALID_VALUE;
      else
        hints.stencil_bits = value;
      break;
    case GLFW_SAMPLES:
      if (value < 0 && value != GLFW_DONT_CARE)
        err = GLFW_INVALID_VALUE;
      else
        hints.samples = value;
      break;
    case GLFW_SRGB_CAPABLE:
      switch (value)
        {
        case GLFW_TRUE:
          hints.srgb_capable = true;
          break;
        case GLFW_FALSE:
          hints.srgb_capable = false;
          break;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
      break;
    case GLFW_DOUBLEBUFFER:
      switch (value)
        {
        case GLFW_TRUE:
          hints.doublebuffer = true;
          break;
        case GLFW_FALSE:
          hints.doublebuffer = false;
          break;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
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
