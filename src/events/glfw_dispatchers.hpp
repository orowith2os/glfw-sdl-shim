#pragma once

#include <SDL3/SDL_events.h>

void
glfw_window_dispatch_event (GLFWwindow *window, SDL_WindowEvent event);

void
glfw_key_dispatch_event (GLFWwindow *window, SDL_KeyboardEvent event);