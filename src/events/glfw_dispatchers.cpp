#include "helpers/glfw_helpers.hpp"
#include "window/glfw_window.hpp"
#include "glfw_dispatchers.hpp"

void
glfw_window_dispatch_event (SDL_WindowEvent event)
{
  GLFWwindow *window = glfw_window_from_sdl_window_id (event.windowID);
  if (!window)
    return;

  SDL_WindowFlags flags = SDL_GetWindowFlags (window->sdl_window);

  if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
    window->should_close = true;
  else if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
    {
      state.focused_window = window;

      do_thing_with_cursor_mode (window);

      if (window->fullscreen_center_cursor_on_first_focus && (flags & SDL_WINDOW_FULLSCREEN) != 0)
        {
          int w = 0;
          int h = 0;
          glfwGetWindowSize (window, &w, &h);
          SDL_WarpMouseInWindow (window->sdl_window, w/2.0f, h/2.0f);
          window->fullscreen_center_cursor_on_first_focus = false;
        }
    }
  else
    glfw_cursor_focus_dispatch_event (event);
}

void
glfw_key_dispatch_event (SDL_KeyboardEvent event)
{
  GLFWwindow *window = glfw_window_from_sdl_window_id (event.windowID);

  if (!window)
    return;

  int action = event.down
                 ? (event.repeat ? GLFW_REPEAT : GLFW_PRESS)
                 : GLFW_RELEASE;

  if (window->key_callback)
    window->key_callback (window,
                          translate_sdl_scancode_to_glfw (event.scancode),
                          event.raw, action,
                          translate_sdl_keymod_to_glfw(window, event.mod));
}
