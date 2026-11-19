#pragma once

#include <SDL3/SDL_events.h>

void
glfw_window_dispatch_event (SDL_WindowEvent event);

void
glfw_key_dispatch_event (SDL_KeyboardEvent event);

void
glfw_cursor_focus_dispatch_event (SDL_WindowEvent event);