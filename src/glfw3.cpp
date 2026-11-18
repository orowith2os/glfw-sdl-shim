#include "glfw3.h"
#include "config.h"
#include "glfw_helpers.hpp"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_vulkan.h>

#include <string>
#include <format>
#include <atomic>
#include <thread>
#include <print>

extern "C" {

int
glfwInit ()
{
  int err = GLFW_NO_ERROR;
  const char *error_ctx = "";

  if (state.initialized)
    return GLFW_TRUE;

  if (!SDL_InitSubSystem (SDL_INIT_VIDEO))
    {
      error_ctx = SDL_GetError ();
      state.failed_sdl_init = true;
      err = GLFW_PLATFORM_ERROR;
    }

  if (!err)
    {
      state.initialized = true;
      // TODO
      state.selected_glfw_platform = GLFW_PLATFORM_WAYLAND;
      return GLFW_TRUE;
    }
  else
    {
      std::string_view ctx = error_ctx;
      do_error_things (err, ctx);

      glfwTerminate ();
      return GLFW_FALSE;
    }
}

void
glfwInitHint (int hint, int value)
{
  int err = GLFW_NO_ERROR;
  std::string_view ctx = "";

  switch (hint)
    {
/* Shared */
    case GLFW_PLATFORM:
      switch (value)
        {
        case GLFW_PLATFORM_WAYLAND:
          // TODO
        case GLFW_ANY_PLATFORM:
          return;
        case GLFW_PLATFORM_NULL:
        case GLFW_PLATFORM_X11:
        case GLFW_PLATFORM_WIN32:
        case GLFW_PLATFORM_COCOA:
          // not supported yet
          err = GLFW_INVALID_VALUE;
          ctx = "Platforms other than Wayland aren't supported yet";
          break;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
      break;
    case GLFW_JOYSTICK_HAT_BUTTONS:
      switch (value)
        {
        case GLFW_TRUE:
        case GLFW_FALSE:
          // TODO?
          return;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
      break;
    case GLFW_ANGLE_PLATFORM_TYPE:
      switch (value)
        {
        case GLFW_ANGLE_PLATFORM_TYPE_NONE:
          return;
        case GLFW_ANGLE_PLATFORM_TYPE_OPENGL:
        case GLFW_ANGLE_PLATFORM_TYPE_OPENGLES:
        case GLFW_ANGLE_PLATFORM_TYPE_D3D9:
        case GLFW_ANGLE_PLATFORM_TYPE_D3D11:
        case GLFW_ANGLE_PLATFORM_TYPE_VULKAN:
        case GLFW_ANGLE_PLATFORM_TYPE_METAL:
          err = GLFW_INVALID_VALUE;
          ctx = "ANGLE isn't supported yet";
          break;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
      break;

/* macOS */
    case GLFW_COCOA_CHDIR_RESOURCES:
    case GLFW_COCOA_MENUBAR:
      switch (value)
        {
        case GLFW_TRUE:
        case GLFW_FALSE:
        default:
          return;
        }
      break;

/* Wayland */
    case GLFW_WAYLAND_LIBDECOR:
      switch (value)
        {
        // This is a bit weird. ALLOW is probably what the GLFW dev intended.
        case GLFW_WAYLAND_PREFER_LIBDECOR:
           if (!SDL_SetHint (SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, "1"))
             {
               err = GLFW_PLATFORM_ERROR;
               ctx = SDL_GetError ();
             }
          break;
        case GLFW_WAYLAND_DISABLE_LIBDECOR:
           if (!SDL_SetHint (SDL_HINT_VIDEO_WAYLAND_ALLOW_LIBDECOR, "0"))
             {
               err = GLFW_PLATFORM_ERROR;
               ctx = SDL_GetError ();
             }
          break;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
      break;

/* X11 */
    case GLFW_X11_XCB_VULKAN_SURFACE:
      switch (value)
        {
        case GLFW_TRUE:
        case GLFW_FALSE:
          // TODO
          return;
        default:
          err = GLFW_INVALID_VALUE;
          break;
        }
      break;

    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Unrecognized init hint";
      break;
    }

  if (err == GLFW_INVALID_VALUE && ctx.empty())
    ctx = "Invalid value for hint";

  do_error_things (err, ctx);
}

void
glfwTerminate ()
{
  if (!state.initialized)
    return;

  if (state.vulkan_loaded)
    SDL_Vulkan_UnloadLibrary ();

  if (!state.failed_sdl_init)
    SDL_QuitSubSystem (SDL_INIT_VIDEO);

  state.initialized = false;
}

// The original GLFW method resets the error on read.
int
glfwGetError (const char **description)
{
  int last_err_code = thread_state.last_api_error;
  thread_state.last_api_error = GLFW_NO_ERROR;

  if (description)
    {
      if (last_err_code && !thread_state.last_api_error_ctx.empty())
        *description = thread_state.last_api_error_ctx.c_str ();
      else
        *description = nullptr;
    }

  return last_err_code;
}

void
glfwInitAllocator (const GLFWallocator *)
{
  std::println ("Silently ignoring glfwInitAllocator, not compatible with SDL...");
}

int
glfwGetPlatform ()
{
  return state.selected_glfw_platform;
}

int
glfwPlatformSupported (int platform)
{
  switch (platform)
    {
    case GLFW_PLATFORM_WAYLAND:
      return GLFW_TRUE;
    // TODO
    default:
      return GLFW_FALSE;
    }
}

GLFWerrorfun
glfwSetErrorCallback (GLFWerrorfun callback)
{
  GLFWerrorfun old_callback = state.err_callback;

  state.err_callback = callback;

  return old_callback;
}

void
glfwGetVersion (int *major, int *minor, int *rev)
{
  if (major)
    *major = GLFW_MAJOR_VERSION;
  if (minor)
    *minor = GLFW_MINOR_VERSION;
  if (rev)
    *rev = GLFW_PATCH_VERSION;
}

const char*
glfwGetVersionString ()
{
  // This (can) have some debug info that may be useful to users and devs
  return SHIM_VERSION_STRING;
}

} // extern "C"
