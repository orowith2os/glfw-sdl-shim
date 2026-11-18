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
                  const char* title,
                  GLFWmonitor* monitor,
                  GLFWwindow* share)
{
  glfwInit ();
  ASSERT_MAIN_THREAD_PTR ()

  GLFWwindow *window = new (std::nothrow) GLFWwindow;
  SDL_WindowFlags flags = hints.flags;

  if (!window)
    {
      do_error_things (GLFW_PLATFORM_ERROR, "Failed to allocate a GLFWwindow.");
      return nullptr;
    }

  if (monitor)
    flags |= SDL_WINDOW_FULLSCREEN;

  bool result = run_on_main_sync([&]() {
    window->sdl_window = SDL_CreateWindow (title, width, height, flags);
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
