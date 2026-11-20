#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_events.h>
#include "cursor/glfw_cursor.hpp"
#include <algorithm>

void
do_thing_with_cursor_mode (GLFWwindow *window)
{
  if (!window || !window->sdl_window)
    return;

  switch (window->cursor_mode)
    {
    case GLFW_CURSOR_NORMAL:
      SDL_SetWindowRelativeMouseMode (window->sdl_window, false);
      SDL_SetWindowMouseGrab (window->sdl_window, false);
      SDL_ShowCursor ();
      break;

    case GLFW_CURSOR_HIDDEN:
      SDL_SetWindowRelativeMouseMode (window->sdl_window, false);
      SDL_SetWindowMouseGrab (window->sdl_window, false);
      if (SDL_GetWindowFlags (window->sdl_window) & SDL_WINDOW_MOUSE_FOCUS)
        SDL_HideCursor ();
      else
        SDL_ShowCursor ();
      break;

    // TODO: ????? why does this not work???
    case GLFW_CURSOR_CAPTURED:
      SDL_ShowCursor ();
      SDL_SetWindowRelativeMouseMode (window->sdl_window, false);
      SDL_SetWindowMouseGrab (window->sdl_window, true);
      break;

    case GLFW_CURSOR_DISABLED:
      SDL_HideCursor ();
      SDL_SetWindowMouseGrab (window->sdl_window, false);
      SDL_SetWindowRelativeMouseMode (window->sdl_window, true);
      break;
    }
}

void
glfw_cursor_focus_dispatch_event (SDL_WindowEvent event)
{
  if (event.type == SDL_EVENT_WINDOW_MOUSE_ENTER)
    {
      auto window = glfw_window_from_sdl_window_id (event.windowID);
      if (window)
        SDL_SetCursor (window->cursor
                         ? window->cursor->sdl_cursor
                         : SDL_GetDefaultCursor ());
      do_thing_with_cursor_mode (window);
    }
}

extern "C" {

void
glfwGetCursorPos (GLFWwindow* window, double* xpos, double* ypos)
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
      do_error_things (GLFW_INVALID_VALUE, "Didn't pass a window to glfwGetCursorPos.");
      return;
    }

  float x = 0.0f, y = 0.0f;

  if (SDL_GetMouseFocus () == window->sdl_window)
    {
      SDL_GetMouseState (&x, &y);
    }
  else
    {
      int win_x = 0, win_y = 0;
      if (!SDL_GetWindowPosition (window->sdl_window, &win_x, &win_y))
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          return;
        }

      float global_x = 0.0f, global_y = 0.0f;
      SDL_GetGlobalMouseState (&global_x, &global_y);

      x = global_x - static_cast<float> (win_x);
      y = global_y - static_cast<float> (win_y);
    }

  if (xpos)
    *xpos = static_cast<double> (x);
  if (ypos)
    *ypos = static_cast<double> (y);
}

void
glfwSetCursorPos (GLFWwindow *window, double xpos, double ypos)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_MAIN_THREAD_VOID ()
  if (!window)
    return;

  SDL_WarpMouseInWindow (window->sdl_window, xpos, ypos);
}

GLFWcursor *
glfwCreateCursor (const GLFWimage *image, int xhot, int yhot)
{
  ASSERT_MAIN_THREAD_PTR ()
  ASSERT_GLFW_INIT_PTR ()

  auto surface = sdl_surface_from_glfw_image (image);
  if (!surface)
    return nullptr;

  auto glfw_cursor = new (std::nothrow) GLFWcursor ();
  if (!glfw_cursor)
    {
      SDL_DestroySurface (surface);
      return nullptr;
    }

  SDL_Cursor* cursor = SDL_CreateColorCursor (surface, xhot, yhot);
  if (!cursor)
    {
      delete glfw_cursor;
      SDL_DestroySurface (surface);
      return nullptr;
    }

  SDL_DestroySurface (surface);

  glfw_cursor->sdl_cursor = cursor;
  return glfw_cursor;
}

GLFWcursor *
glfwCreateStandardCursor (int shape)
{
  ASSERT_MAIN_THREAD_PTR ()
  ASSERT_GLFW_INIT_PTR ()

  auto cursor = new (std::nothrow) GLFWcursor ();
  if (!cursor)
    return nullptr;

  cursor->sdl_cursor =
    SDL_CreateSystemCursor (glfw_cursor_int_to_sdl_system_cursor (shape));

  if (!cursor->sdl_cursor)
    {
      delete cursor;
      return nullptr;
    }

  return cursor;
}

void
glfwDestroyCursor (GLFWcursor *cursor)
{
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_GLFW_INIT_VOID ()

  if (!cursor)
    return;

  for (auto window : cursor->using_windows)
    {
      window->cursor = nullptr;
      if (SDL_GetMouseFocus () == window->sdl_window)
        SDL_SetCursor (SDL_GetDefaultCursor ());
    }

  delete cursor;
}

void
glfwSetCursor (GLFWwindow *window, GLFWcursor *cursor)
{
  ASSERT_MAIN_THREAD_VOID ()
  ASSERT_GLFW_INIT_VOID ()

  if (!window || window->cursor == cursor)
    return;

  if (window->cursor)
    std::erase (window->cursor->using_windows, window);

  window->cursor = cursor;
  if (cursor)
    cursor->using_windows.push_back (window);

  if (SDL_GetMouseFocus () == window->sdl_window)
    SDL_SetCursor (cursor ? cursor->sdl_cursor : SDL_GetDefaultCursor ());
}

} // extern "C"
