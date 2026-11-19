#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_events.h>
#include "cursor/glfw_cursor.hpp"
#include <algorithm>

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
    }
}

extern "C" {

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
