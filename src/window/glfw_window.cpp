#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include "glfw_window.hpp"

void
glfwDestroyWindow (GLFWwindow *window)
{
  ASSERT_MAIN_THREAD_VOID ()
  delete window;
}

int
glfwWindowShouldClose (GLFWwindow *window) {
  if (!window)
    {
      do_error_things(GLFW_INVALID_VALUE, "No window was passed to glfwWindowShouldClose");
      // We want them to discard it.
      return GLFW_TRUE;
    }

  return window->should_close ? GLFW_TRUE : GLFW_FALSE;
}

GLFWwindow *
glfwCreateWindow (int width, int height,
                  const char *title,
                  GLFWmonitor *monitor,
                  GLFWwindow *share)
{
  glfwInit ();
  ASSERT_MAIN_THREAD_PTR ()

  int context_flags = 0;
  GLFWwindow *window = new (std::nothrow) GLFWwindow;
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

    if (hints.debug_context)
      context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;

    if (hints.forward_compat)
      context_flags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;

  bool result = run_on_main_sync([&]() {
    window->sdl_window = SDL_CreateWindow (title, width, height, flags);

    if (!window->sdl_window)
      {
        do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError());
        delete window;
        window = nullptr;
      }

    // Don't bother with GL if the client is handling it.
    if (hints.client_api != GLFW_NO_API)
      return;

    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, hints.opengl_version_major);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, hints.opengl_version_minor);
    SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, hints.doublebuffer);
    SDL_GL_SetAttribute (SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, hints.srgb_capable);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, context_flags);

    // Bits. max them in case of GLFW_DONT_CARE
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     std::max(0, hints.red_bits));
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   std::max(0, hints.green_bits));
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    std::max(0, hints.blue_bits));
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   std::max(0, hints.alpha_bits));
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   std::max(0, hints.depth_bits));
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, std::max(0, hints.stencil_bits));
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
  });

  if (!result || !window->sdl_window)
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      delete window;
      return nullptr;
    }

  return window;
}

void
glfwMakeContextCurrent (GLFWwindow *window)
{
  glfwInit ();

  if (!window->gl_context)
    do_error_things (GLFW_NO_WINDOW_CONTEXT, "Window did not have an OpenGL context.");

  run_on_main_sync([&]()
    {
      if (!SDL_GL_MakeCurrent(window->sdl_window, window->gl_context))
        do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
    });
}
