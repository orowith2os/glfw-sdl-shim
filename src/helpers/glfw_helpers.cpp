#include "glfw3.h"
#include "glfw_helpers.hpp"

#include <format>

SDL_Surface *
sdl_surface_from_glfw_image (const GLFWimage *image)
{
  if (!image || !image->pixels || image->width <= 0 || image->height <= 0)
    return nullptr;

  return SDL_CreateSurfaceFrom (image->width,
                                image->height,
                                SDL_PIXELFORMAT_RGBA32,
                                image->pixels,
                                image->width * 4);
}

void
glfw_window_from_sdl_window (SDL_Window *sdl_window,
                             GLFWwindow **window)
{
  SDL_PropertiesID props = SDL_GetWindowProperties (sdl_window);
  *window =
    static_cast<GLFWwindow*> (SDL_GetPointerProperty (props,
                                                      "gay.oro.glfw_window_pointer",
                                                      nullptr));
}

void
glfw_monitor_from_sdl_display_id (SDL_DisplayID sdl_display,
                                  GLFWmonitor **monitor)
{
  SDL_PropertiesID props = SDL_GetDisplayProperties (sdl_display);
  *monitor =
    static_cast<GLFWmonitor*> (SDL_GetPointerProperty (props,
                                                       "gay.oro.glfw_monitor_pointer",
                                                       nullptr));
}

GLFWwindow *
glfw_window_from_sdl_window_id (SDL_WindowID id)
{
  SDL_Window *sdl_window = SDL_GetWindowFromID (id);
  GLFWwindow *window = nullptr;

  if (!sdl_window)
    return nullptr;

  glfw_window_from_sdl_window (sdl_window, &window);

  return window;
}

// https://www.glfw.org/docs/latest/group__errors.html
std::optional<std::string_view>
err_to_string (int err)
{
#define CASE(x) case x: return #x
  switch (err)
    {
      CASE (GLFW_NO_ERROR);
      CASE (GLFW_NOT_INITIALIZED);
      CASE (GLFW_NO_CURRENT_CONTEXT);
      CASE (GLFW_INVALID_ENUM);
      CASE (GLFW_INVALID_VALUE);
      CASE (GLFW_OUT_OF_MEMORY);
      CASE (GLFW_API_UNAVAILABLE);
      CASE (GLFW_VERSION_UNAVAILABLE);
      CASE (GLFW_PLATFORM_ERROR);
      CASE (GLFW_FORMAT_UNAVAILABLE);
      CASE (GLFW_NO_WINDOW_CONTEXT);
      CASE (GLFW_CURSOR_UNAVAILABLE);
      CASE (GLFW_FEATURE_UNAVAILABLE);
      CASE (GLFW_FEATURE_UNIMPLEMENTED);
      CASE (GLFW_PLATFORM_UNAVAILABLE);
    default:
      return std::nullopt;
    }
#undef CASE
}

void
do_error_things (int err, std::string_view ctx)
{
  std::string ctx_clarified;

  if (!err)
    return; // keep errors around until we actually get one, just like real GLFW

  thread_state.last_api_error = err;
  auto error_ctx = err_to_string (err);
  if (error_ctx)
    {
      ctx_clarified += *error_ctx;
      if (!ctx.empty())
        {
          ctx_clarified += ": ";
          ctx_clarified += ctx;
        }
      else
        ctx_clarified += ": No additional context for the error was provided.";
    }
  else
    {
      ctx_clarified += std::format ("Unknown integer value ({}), please file a bug.", err);
      if (!ctx.empty())
        {
          ctx_clarified += " Additional error context: ";
          ctx_clarified += ctx;
        }
      else
        ctx_clarified += " No additional context for the error was provided.";
    }

  thread_state.last_api_error_ctx = ctx_clarified;

  if (state.err_callback)
    state.err_callback (err, thread_state.last_api_error_ctx.c_str ());
}

int
assert_main_thread ()
{
  if (!glfwInit ())
    return GLFW_FALSE;
  if (!SDL_IsMainThread())
    {
      do_error_things (GLFW_INVALID_VALUE, "Called glfwDefaultWindowHints outside of the main thread.");
      return GLFW_FALSE;
    }
  return GLFW_TRUE;
}

int
assert_glfw_init ()
{
  if (state.initialized)
    return GLFW_TRUE;
  else
    {
      do_error_things (GLFW_NOT_INITIALIZED, "GLFW was not initialized.");
      return GLFW_FALSE;
    }
}

bool
is_printable_glfw_key (int key)
{
  return key == GLFW_KEY_KP_EQUAL
      || (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_ADD)
      || (key >= GLFW_KEY_APOSTROPHE && key <= GLFW_KEY_WORLD_2);
}
