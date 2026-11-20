#pragma once

#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include <SDL3/SDL_video.h>

bool
glfw_monitor_update_current_mode (GLFWmonitor *monitor);

struct GLFWmonitor
{
  SDL_DisplayID sdl_displayid = 0;
  // GLFW requires us to hold this for the entire lifetime of the monitor,
  // the caller accesses it (then we can refresh), or termination
  GLFWvidmode current_mode = {};
  void *user_pointer = nullptr;

  ~GLFWmonitor ()
    {
      std::erase (state.monitors, this);
    }
};