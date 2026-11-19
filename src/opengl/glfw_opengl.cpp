#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include "window/glfw_window.hpp"

void
glfwSwapBuffers (GLFWwindow *window)
{
  glfwInit ();
  run_on_main_sync([&]()
    {
      if (!SDL_GL_SwapWindow (window->sdl_window))
        do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
    });
}

// TODO: negative swap intervals, tearing with swap intervals of 0 (Wayland)?
// https://registry.khronos.org/EGL/extensions/EXT/EGL_EXT_swap_tearing_hint.txt
// https://github.com/libsdl-org/SDL/issues/16716
void
glfwSwapInterval (int interval)
{
  glfwInit ();
  run_on_main_sync([&]()
    {
      if (!SDL_GL_SetSwapInterval (interval))
        do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
    });
}
