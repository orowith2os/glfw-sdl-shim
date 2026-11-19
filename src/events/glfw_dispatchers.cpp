#include "helpers/glfw_helpers.hpp"
#include "window/glfw_window.hpp"
#include "glfw_dispatchers.hpp"

void
glfw_window_dispatch_event (SDL_WindowEvent event)
{
  GLFWwindow *window = glfw_window_from_sdl_window_id (event.windowID);

  if (!window)
    return;

  if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
    window->should_close = true;
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
