#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include "window/glfw_window.hpp"

extern "C" {

// This can be called from any thread.
GLFWwindow *
glfwGetCurrentContext ()
{
  ASSERT_GLFW_INIT (nullptr)
  GLFWwindow *window = nullptr;

  run_on_main_sync ([&] ()
    {
      auto sdl_window = SDL_GL_GetCurrentWindow ();
      if (sdl_window)
        glfw_window_from_sdl_window (sdl_window, &window);
      else
        do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
    });

  return window;
}

// This can be called from any thread.
GLFWglproc
glfwGetProcAddress (const char *procname)
{
  ASSERT_GLFW_INIT (nullptr)
  if (!procname)
    return nullptr;

  GLFWglproc ptr = nullptr;

  run_on_main_sync ([&] ()
    {
      if (!SDL_GL_GetCurrentContext ())
        return;

      ptr = SDL_GL_GetProcAddress (procname);
    });

  return ptr;
}

// This can be called from any thread.
int
glfwExtensionSupported (const char *extension)
{
  int val;
  ASSERT_GLFW_INIT (GLFW_FALSE)

  if (!extension || *extension == '\0')
    return GLFW_FALSE;

  run_on_main_sync ([&] ()
    {
      if (!SDL_GL_GetCurrentContext ())
        val = GLFW_FALSE;

      val = SDL_GL_ExtensionSupported (extension) ? GLFW_TRUE : GLFW_FALSE;
    });

  return val;
}

// This can be called from any thread.
void
glfwMakeContextCurrent (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()

  if (!window->gl_context)
    {
      do_error_things (GLFW_NO_WINDOW_CONTEXT, "Window did not have an OpenGL context.");
      return;
    }

  run_on_main_sync ([&] ()
    {
      if (!SDL_GL_MakeCurrent (window->sdl_window, window->gl_context))
        do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
    });
}

// This can be called from any thread.
void
glfwSwapBuffers (GLFWwindow *window)
{
  ASSERT_GLFW_INIT_VOID ()

  run_on_main_sync ([&] ()
    {
      if (!SDL_GL_SwapWindow (window->sdl_window))
        do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
    });
}

// This can be called from any thread.
void
glfwSwapInterval (int interval)
{
  ASSERT_GLFW_INIT_VOID ()

  run_on_main_sync ([&] ()
    {
      if (!SDL_GL_SetSwapInterval (interval))
        // We might not support any tearing extensions.
        if (!SDL_GL_SetSwapInterval (-interval))
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
    });
}

} // extern "C"
