#pragma once

#include "glfw3.h"
#include "SDL3/SDL_mouse.h"
#include <vector>
#include <algorithm>

struct GLFWcursor
{
  SDL_Cursor *sdl_cursor = SDL_GetDefaultCursor ();
  std::vector<GLFWwindow *> using_windows;

  ~GLFWcursor ()
  {
    if (sdl_cursor)
      SDL_DestroyCursor (sdl_cursor);
  }
};

void
do_thing_with_cursor_mode (GLFWwindow *window);

[[nodiscard]] constexpr SDL_SystemCursor
glfw_cursor_int_to_sdl_system_cursor (int cursor) noexcept
{
  switch (cursor)
    {
    case GLFW_ARROW_CURSOR:
      return SDL_SYSTEM_CURSOR_DEFAULT;
    case GLFW_IBEAM_CURSOR:
      return SDL_SYSTEM_CURSOR_TEXT;
    case GLFW_CROSSHAIR_CURSOR:
      return SDL_SYSTEM_CURSOR_CROSSHAIR;
    case GLFW_POINTING_HAND_CURSOR:
      return SDL_SYSTEM_CURSOR_POINTER;
    case GLFW_RESIZE_EW_CURSOR:
      return SDL_SYSTEM_CURSOR_EW_RESIZE;
    case GLFW_RESIZE_NS_CURSOR:
      return SDL_SYSTEM_CURSOR_NS_RESIZE;
    case GLFW_RESIZE_NWSE_CURSOR:
      return SDL_SYSTEM_CURSOR_NWSE_RESIZE;
    case GLFW_RESIZE_NESW_CURSOR:
      return SDL_SYSTEM_CURSOR_NESW_RESIZE;
    case GLFW_RESIZE_ALL_CURSOR:
      return SDL_SYSTEM_CURSOR_MOVE;
    case GLFW_NOT_ALLOWED_CURSOR:
      return SDL_SYSTEM_CURSOR_NOT_ALLOWED;
    default: // SDL_CreateSystemCursor will fail with this
      return SDL_SYSTEM_CURSOR_COUNT;
    }
}