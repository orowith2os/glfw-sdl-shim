#pragma once

#include "glfw3.h"
#include <SDL3/SDL_video.h>

struct GLFWmonitor {
  SDL_DisplayID sdl_displayid;
  void *user_pointer = nullptr;
};