#pragma once

#include "glfw_helpers.hpp"
#include <SDL3/SDL_video.h>

extern "C" {

struct GLFWwindow {
  SDL_Window *sdl_window;
};

} // extern "C"