#include "helpers/glfw_helpers.hpp"
#include "window/glfw_window.hpp"
#include "glfw_dispatchers.hpp"
#include "config.h"

void
glfw_window_dispatch_event (GLFWwindow *window, SDL_WindowEvent event)
{
  ASSERT_VALID_WIN_VOID (window)

  SDL_WindowFlags flags = SDL_GetWindowFlags (window->sdl_window);

  if (event.type == SDL_EVENT_WINDOW_HIDDEN)
    window->hidden = true;
  else if (event.type == SDL_EVENT_WINDOW_SHOWN)
    window->hidden = false;
  else if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
    {
      window->should_close = true;
      if (window->close_callback)
        window->close_callback (window);
    }
  else if (event.type == SDL_EVENT_WINDOW_MOUSE_ENTER)
    {
      window->hovered = true;
      SDL_SetCursor (window->cursor ? window->cursor->sdl_cursor : SDL_GetDefaultCursor ());
      do_thing_with_cursor_mode (window);

      if (window->cursor_enter_callback)
        window->cursor_enter_callback (window, GLFW_TRUE);
    }
  else if (event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE)
    {
      window->hovered = false;
      if (window->cursor_enter_callback)
        window->cursor_enter_callback (window, GLFW_FALSE);
    }
  else if (event.type == SDL_EVENT_WINDOW_RESIZED)
    {
      // data1 == width, data2 == height
      if (window->window_size_callback)
        window->window_size_callback (window, event.data1, event.data2);
    }
  else if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
    {
      // data1 == x, data2 == y
      if (window->framebuffer_size_callback)
        window->framebuffer_size_callback (window, event.data1, event.data2);
    }
  else if (event.type == SDL_EVENT_WINDOW_EXPOSED)
    {
      if (window->window_refresh_callback)
        window->window_refresh_callback (window);
    }
  else if (event.type == SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED)
    {
      if (window->content_scale_callback)
        {
          float scale = SDL_GetWindowDisplayScale (window->sdl_window);
          if (scale == 0.0f)
            scale = 1.0f;

          window->content_scale_callback (window, scale, scale);
        }
    }
  else if (event.type == SDL_EVENT_WINDOW_MOVED)
    {
      // data1 == x, data2 == y
      if (window->window_pos_callback)
        window->window_pos_callback (window, event.data1, event.data2);
    }
  else if (event.type == SDL_EVENT_WINDOW_MAXIMIZED)
    {
      window->maximized = true;
      window->minimized = false;
      if (window->maximized_callback)
        window->maximized_callback (window, GLFW_TRUE);
    }
  else if (event.type == SDL_EVENT_WINDOW_MINIMIZED)
    {
      window->minimized = true;
      window->maximized = false;
      if (window->iconify_callback)
        window->iconify_callback (window, GLFW_TRUE);
    }
  else if (event.type == SDL_EVENT_WINDOW_RESTORED)
    {
      if (window->maximized)
        {
          window->maximized = false;
          if (window->maximized_callback)
            window->maximized_callback (window, GLFW_FALSE);
        }
      else if (window->minimized)
        {
          window->minimized = false;
          if (window->iconify_callback)
            window->iconify_callback (window, GLFW_FALSE);
        }
    }
  else if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST)
    {
      if (window->focus_callback)
        window->focus_callback (window, GLFW_FALSE);
    }
  else if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED)
    {
      state.focused_window = window;
      if (window->focus_callback)
        window->focus_callback (window, GLFW_TRUE);

      do_thing_with_cursor_mode (window);

      if (window->fullscreen_center_cursor_on_first_focus && (flags & SDL_WINDOW_FULLSCREEN) != 0)
        {
          int w = 0;
          int h = 0;
          glfwGetWindowSize (window, &w, &h);
          SDL_WarpMouseInWindow (window->sdl_window, w / 2.0f, h / 2.0f);
          window->fullscreen_center_cursor_on_first_focus = false;
        }
    }
}
