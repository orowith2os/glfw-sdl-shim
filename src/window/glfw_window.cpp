#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include "glfw_window.hpp"

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_clipboard.h>
#include <span>

extern "C" {

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
  *ypos = x;
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
  ASSERT_GLFW_INIT_PTR ()
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
  ASSERT_GLFW_INIT_PTR ()
  ASSERT_MAIN_THREAD_PTR ()

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

  std::span<const GLFWimage> imageSpan (images, static_cast<std::size_t> (count));

  SDL_Surface *valid_surface = nullptr;
  for (const GLFWimage &img : imageSpan)
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
  ASSERT_GLFW_INIT_PTR ()
  ASSERT_MAIN_THREAD_PTR ()
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
    case GLFW_STICKY_KEYS:
    case GLFW_STICKY_MOUSE_BUTTONS:
      return; // TODO
    case GLFW_LOCK_KEY_MODS:
      bool_hint_helper (value, &window->lock_key_mods, &err);
      break;
    case GLFW_RAW_MOUSE_MOTION:
      return; // TODO?
    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Unknown enum variant for SetWindowHint. File a bug.";
    }

  if (err == GLFW_INVALID_VALUE && ctx.empty())
    ctx = "Invalid value for hint";

  do_error_things (err, ctx);
}

GLFWscrollfun
glfwSetScrollCallback (GLFWwindow *window, GLFWscrollfun callback)
{
  ASSERT_GLFW_INIT_PTR ()
  ASSERT_MAIN_THREAD_PTR ()
  if (!window)
    return nullptr;

  GLFWscrollfun previous = window->scroll_callback;
  window->scroll_callback = callback;
  return previous;
}

GLFWkeyfun
glfwSetKeyCallback (GLFWwindow *window, GLFWkeyfun callback)
{
  ASSERT_GLFW_INIT_PTR ()
  ASSERT_MAIN_THREAD_PTR ()

  GLFWkeyfun old_callback = window->key_callback;

  window->key_callback = callback;

  return old_callback;
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
  ASSERT_GLFW_INIT_INT ()

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
  ASSERT_GLFW_INIT_PTR ()
  ASSERT_MAIN_THREAD_PTR ()

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

  if (hints.debug_context)
    context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
  if (hints.forward_compat)
    context_flags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;


  bool result = run_on_main_sync ([&]()
    {
      window->sdl_window = SDL_CreateWindow (title, width, height, flags);

      if (!window->sdl_window)
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          delete window;
          window = nullptr;
        }

      // Don't bother with GL if the client is handling it.
      if (hints.client_api == GLFW_NO_API)
        return;

      SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, hints.opengl_version_major);
      SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, hints.opengl_version_minor);
      SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, hints.doublebuffer);
      SDL_GL_SetAttribute (SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, hints.srgb_capable);
      SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, context_flags);

      // Bits. max them in case of GLFW_DONT_CARE
      SDL_GL_SetAttribute (SDL_GL_RED_SIZE,     std::max(0, hints.red_bits));
      SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE,   std::max(0, hints.green_bits));
      SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE,    std::max(0, hints.blue_bits));
      SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE,   std::max(0, hints.alpha_bits));
      SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE,   std::max(0, hints.depth_bits));
      SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, std::max(0, hints.stencil_bits));
      SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, (hints.samples > 0)
                                                       ? 1 : 0);

      SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, std::max (0, hints.samples));

      if (share && share->gl_context)
        {
          if (!SDL_GL_MakeCurrent (share->sdl_window, share->gl_context))
            {
              delete window;
              window = nullptr;
              do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
            }

          SDL_GL_SetAttribute (SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
        }

      window->gl_context = SDL_GL_CreateContext (window->sdl_window);
      SDL_GL_SetAttribute (SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);

      SDL_GL_MakeCurrent(window->sdl_window, window->gl_context);

      SDL_PropertiesID props = SDL_GetWindowProperties (window->sdl_window);
      SDL_SetPointerProperty (props, "gay.oro.glfw_window_pointer", window);
    });

  if (!result || !window->sdl_window)
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      delete window;
      return nullptr;
    }

  return window;
}

} // extern "C"
