#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include "glfw_window.hpp"
#include "monitor/glfw_monitor.hpp"

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_clipboard.h>
#include <SDL3/SDL_platform.h>
#include <span>
#include <format>

extern "C" {

void
glfwSetWindowMonitor (GLFWwindow *window,
                      GLFWmonitor *monitor,
                      int xpos, int ypos,
                      int width, int height,
                      int refreshRate)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)

  if (width <= 0 || height <= 0)
    {
      do_error_things (GLFW_INVALID_VALUE,
                       "Invalid window size passed to glfwSetWindowMonitor.");
      return;
    }

  if (refreshRate < 0 && refreshRate != GLFW_DONT_CARE)
    {
      do_error_things (GLFW_INVALID_VALUE,
                       "Invalid refresh rate passed to glfwSetWindowMonitor.");
      return;
    }

  SDL_Window *sdl_win = window->sdl_window;
  bool was_fullscreen = (SDL_GetWindowFlags (sdl_win) & SDL_WINDOW_FULLSCREEN) != 0;

  if (!monitor)
    {
      if (was_fullscreen)
        {
          if (!SDL_SetWindowFullscreen (sdl_win, false))
            {
              do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
              return;
            }
        }

      if (!SDL_SetWindowSize (sdl_win, width, height))
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          return;
        }

      if (!SDL_SetWindowPosition (sdl_win, xpos, ypos))
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          return;
        }
    }
  else
    {
      SDL_DisplayID display_id = monitor->sdl_displayid;

      // Get it as close as possible, if possible
      SDL_Rect bounds{};
      if (SDL_GetDisplayBounds (display_id, &bounds))
        SDL_SetWindowPosition (sdl_win, bounds.x, bounds.y);

      float target_refresh = (refreshRate == GLFW_DONT_CARE)
                               ? 0.0f
                               : static_cast<float> (refreshRate);

      SDL_DisplayMode closest_mode{};
      if (!SDL_GetClosestFullscreenDisplayMode (display_id,
                                                width, height,
                                                target_refresh,
                                                true,
                                                &closest_mode))
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          return;
        }

      if (!SDL_SetWindowFullscreenMode (sdl_win, &closest_mode))
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          return;
        }

      if (!was_fullscreen)
        {
          if (!SDL_SetWindowFullscreen (sdl_win, true))
            {
              do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
              return;
            }
        }
    }
}

GLFWmonitor *
glfwGetWindowMonitor (GLFWwindow *window)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)
  ASSERT_VALID_WIN (window, nullptr)

  SDL_WindowFlags flags = SDL_GetWindowFlags (window->sdl_window);
  if (!(flags & SDL_WINDOW_FULLSCREEN))
    return nullptr;

  SDL_DisplayID display_id = SDL_GetDisplayForWindow (window->sdl_window);
  if (display_id == 0)
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      return nullptr;
    }

  GLFWmonitor *monitor = nullptr;
  glfw_monitor_from_sdl_display_id (display_id, &monitor);
  return monitor;
}

void
glfwGetWindowFrameSize (GLFWwindow *window,
                        int *left, int *top, int *right, int *bottom)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)

  SDL_GetWindowBordersSize (window->sdl_window, top, left, bottom, right);
}

void
glfwGetWindowContentScale (GLFWwindow *window,
                           float *xscale, float *yscale)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)
  if (!xscale && !yscale)
    return;

  float scale = SDL_GetWindowDisplayScale (window->sdl_window);

  if (xscale)
    *xscale = scale;
  if (yscale)
    *yscale = scale;
}

void
glfwRestoreWindow (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)

  SDL_RestoreWindow (window->sdl_window);
}

void
glfwMaximizeWindow (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)

  SDL_MaximizeWindow (window->sdl_window);
}

void
glfwIconifyWindow (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)

  SDL_MinimizeWindow (window->sdl_window);
}

float
glfwGetWindowOpacity (GLFWwindow *window)
{
  ASSERT_GLFW_INIT (1.0)
  ASSERT_MAIN_THREAD (1.0)
  ASSERT_VALID_WIN (window, 1.0)

  return SDL_GetWindowOpacity (window->sdl_window);
}

void
glfwSetWindowOpacity (GLFWwindow *window, float opacity)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_VALID_WIN_VOID (window)

  SDL_SetWindowOpacity (window->sdl_window, opacity);
}

void
glfwGetWindowPos (GLFWwindow* window, int* xpos, int* ypos)
{
  if (xpos)
    *xpos = 0;
  if (ypos)
    *ypos = 0;
  if (!xpos && !ypos)
    return;

  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()

  if (!window)
    {
      do_error_things (GLFW_INVALID_VALUE, "Didn't pass a window to glfwGetWindowPos.");
      return;
    }

  int x = 0, y = 0;
  if (!SDL_GetWindowPosition (window->sdl_window, &x, &y))
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      return;
    }

  *xpos = x;
  *ypos = y;
}

void
glfwSetWindowPos (GLFWwindow* window, int xpos, int ypos)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window)
    return;

  if (!SDL_SetWindowPosition (window->sdl_window, xpos, ypos))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}

void
glfwHideWindow (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window)
    return;

  if (!SDL_HideWindow (window->sdl_window))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}

void
glfwGetWindowSize (GLFWwindow *window, int *width, int *height)
{
  if (width)
    *width = 0;
  if (height)
    *height = 0;
  if (!width && !height)
    return;

  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()

  if (!window)
    {
      do_error_things (GLFW_INVALID_VALUE, "Didn't pass a window to glfwGetWindowSize.");
      return;
    }

  int w = 0, h = 0;
  if (!SDL_GetWindowSize (window->sdl_window, &w, &h))
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      return;
    }

  *width = w;
  *height = h;
}

void
glfwSetWindowSize (GLFWwindow *window, int width, int height)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window)
    return;

  if (!SDL_SetWindowSize (window->sdl_window, width, height))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}

void
glfwSetWindowUserPointer (GLFWwindow *window, void *pointer)
{
  ASSERT_GLFW_INIT_VOID ()

  run_on_main_sync ([&]()
    {
      window->user_pointer = pointer;
    });
}

void *
glfwGetWindowUserPointer (GLFWwindow *window)
{
  ASSERT_GLFW_INIT (nullptr)
  void *ptr = nullptr;

  run_on_main_sync ([&]()
    {
      ptr = window->user_pointer;
    });

  return ptr;
}

void
glfwRequestWindowAttention (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window)
    return;

  if (!SDL_FlashWindow (window->sdl_window, SDL_FLASH_UNTIL_FOCUSED))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}

void
glfwSetClipboardString (GLFWwindow *, const char* string)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()

  SDL_SetClipboardText (string);
}

const char *
glfwGetClipboardString (GLFWwindow *)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)

  auto text = SDL_GetClipboardText ();
  if (!text || text[0] == '\0')
    {
      SDL_free (text);
      text = nullptr;
    }

  return text;
}

void
glfwFocusWindow (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()

  SDL_RaiseWindow (window->sdl_window);
}

void
glfwSetWindowIcon (GLFWwindow *window, int count, const GLFWimage *images)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window || !window->sdl_window || count < 0)
    return;

  if (count == 0 || !images)
    return; // SDL doesn't have a "default" concept

  std::span<const GLFWimage> image_span (images, static_cast<std::size_t> (count));

  SDL_Surface *valid_surface = nullptr;
  for (const GLFWimage& img : image_span)
    {
      auto surface = sdl_surface_from_glfw_image (&img);

      if (!surface)
        continue;
      else if (!valid_surface)
        {
          valid_surface = surface;
          continue;
        }
      else
        SDL_AddSurfaceAlternateImage (valid_surface, surface);

      SDL_DestroySurface (surface);
    }

  if (valid_surface)
    {
      SDL_SetWindowIcon (window->sdl_window, valid_surface);
      SDL_DestroySurface (valid_surface);
    }
}

const char *
glfwGetWindowTitle (GLFWwindow *window)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)
  if (!window || !window->sdl_window)
    return nullptr;

  return SDL_GetWindowTitle (window->sdl_window);
}

void
glfwSetWindowTitle (GLFWwindow *window, const char *title)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window || !window->sdl_window)
    return;

  SDL_SetWindowTitle (window->sdl_window, title ? title : "");
}

void
glfwShowWindow (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window || !window->sdl_window)
    return;

  SDL_WindowFlags flags = SDL_GetWindowFlags (window->sdl_window);

  // no-op if already visible or fullscreen
  if (!(flags & SDL_WINDOW_HIDDEN) || (flags & SDL_WINDOW_FULLSCREEN))
    return;

  // GLFW_FOCUS_ON_SHOW
  SDL_SetHint (SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, window->focus_on_show ? "1" : "0");

  SDL_ShowWindow (window->sdl_window);

  SDL_SetHint (SDL_HINT_WINDOW_ACTIVATE_WHEN_SHOWN, "1");
}

void
glfwGetFramebufferSize (GLFWwindow *window,
                        int *width, int *height)
{

  if (width)
    *width = 0;
  if (height)
    *height = 0;
  if (!width && !height)
    return;

  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()

  if (!window)
    {
      do_error_things (GLFW_INVALID_VALUE, "Didn't pass a window to glfwGetFramebufferSize.");
      return;
    }

  int w = 0, h = 0;
  if (!SDL_GetWindowSizeInPixels (window->sdl_window, &w, &h))
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      return;
    }

  *width = w;
  *height = h;
}

void
glfwDestroyWindow (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (window)
    delete window;
}

// This can be called from any thread.
int
glfwWindowShouldClose (GLFWwindow *window)
{
  ASSERT_GLFW_INIT (GLFW_FALSE)

  int val;
  if (!window)
    {
      do_error_things (GLFW_INVALID_VALUE, "No window was passed to glfwWindowShouldClose");
      // We want them to discard it.
      return GLFW_TRUE;
    }

  run_on_main_sync ([&]()
    {
      val = window->should_close ? GLFW_TRUE : GLFW_FALSE;
    });

  return val;
}

// This can be called from any thread.
void
glfwSetWindowShouldClose (GLFWwindow *window, int value)
{
  ASSERT_GLFW_INIT_VOID ()

  if (!window)
    do_error_things (GLFW_INVALID_VALUE, "No window was passed to glfwSetWindowShouldClose");
  else
    run_on_main_sync ([&]()
      {
        window->should_close = (value == GLFW_TRUE) ? true : false;
      });
}

GLFWwindow *
glfwCreateWindow (int width, int height,
                  const char *title,
                  GLFWmonitor *monitor,
                  GLFWwindow *share)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)

  int context_flags = 0;
  GLFWwindow *window = new (std::nothrow) GLFWwindow ();
  SDL_WindowFlags flags = hints.flags;

  if (!window)
    {
      do_error_things (GLFW_PLATFORM_ERROR, "Failed to allocate a GLFWwindow.");
      return nullptr;
    }

  if (monitor)
    flags |= SDL_WINDOW_FULLSCREEN;

  switch (hints.client_api)
    {
    case GLFW_OPENGL_API:
    case GLFW_OPENGL_ES_API:
      flags |= SDL_WINDOW_OPENGL;
      break;
    case GLFW_NO_API:
      break; // client handles this itself
    }

  window->client_api = hints.client_api;
  window->focus_on_show = hints.focus_on_show;
  window->fullscreen_center_cursor_on_first_focus =
    monitor ? hints.fullscreen_center_cursor_on_first_focus : false;
  window->flags = flags;

  if (hints.debug_context)
    context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
  if (hints.forward_compat)
    context_flags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

  if (hints.client_api != GLFW_NO_API)
    {
      SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, hints.opengl_version_major);
      SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, hints.opengl_version_minor);
      SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, hints.doublebuffer);
      SDL_GL_SetAttribute (SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, hints.srgb_capable);
      SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, context_flags);

      // Bits. max them in case of GLFW_DONT_CARE
      SDL_GL_SetAttribute (SDL_GL_RED_SIZE,     std::max (0, hints.red_bits));
      SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE,   std::max (0, hints.green_bits));
      SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE,    std::max (0, hints.blue_bits));
      SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE,   std::max (0, hints.alpha_bits));
      SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE,   std::max (0, hints.depth_bits));
      SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, std::max (0, hints.stencil_bits));
      SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, (hints.samples > 0) ? 1 : 0);
      SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, std::max (0, hints.samples));
    }

  window->sdl_window = SDL_CreateWindow (title, width, height, flags);

  if (!window->sdl_window)
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      delete window;
      return nullptr;
    }

  if (hints.client_api != GLFW_NO_API)
    {
      if (share && share->gl_context)
        {
          if (!SDL_GL_MakeCurrent (share->sdl_window, share->gl_context))
            {
              do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
              SDL_DestroyWindow (window->sdl_window);
              delete window;
              return nullptr;
            }

          SDL_GL_SetAttribute (SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
        }

      window->gl_context = SDL_GL_CreateContext (window->sdl_window);
      SDL_GL_SetAttribute (SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);

      if (!window->gl_context)
        {
          do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
          SDL_DestroyWindow (window->sdl_window);
          delete window;
          return nullptr;
        }

      SDL_GL_MakeCurrent (window->sdl_window, window->gl_context);
    }

  SDL_PropertiesID props = SDL_GetWindowProperties (window->sdl_window);
  SDL_SetPointerProperty (props, "gay.oro.glfw_window_pointer", window);

  return window;
}

} // extern "C"
