#include "helpers/glfw_helpers.hpp"
#include <cmath>
#include <chrono>

#include <SDL3/SDL_events.h>
#include "window/glfw_window.hpp"
#include "monitor/glfw_monitor.hpp"
#include "glfw_dispatchers.hpp"

static Uint32 shim_wakeup_event_type = 0;

bool
glfw_init_events ()
{
  if (shim_wakeup_event_type == 0)
    shim_wakeup_event_type = SDL_RegisterEvents (1);
  return shim_wakeup_event_type != 0;
}

static void
dispatch_event (SDL_Event event)
{
  GLFWwindow *window = nullptr;
  glfw_window_from_sdl_window (SDL_GetWindowFromEvent (&event), &window);

  if (event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST)
    glfw_window_dispatch_event (window, event.window);
  else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP)
    glfw_key_dispatch_event (window, event.key);
  else if (event.type == SDL_EVENT_DISPLAY_ADDED)
    {
      SDL_DisplayID sdl_monitor = event.display.displayID;
      auto monitor = glfw_monitor_from_sdl_display_id (sdl_monitor);
      state.monitors.push_back (monitor);

      if (monitor && state.monitor_callback)
        state.monitor_callback (monitor, GLFW_CONNECTED);
    }
  else if (event.type == SDL_EVENT_DISPLAY_REMOVED)
    {
      SDL_DisplayID sdl_monitor = event.display.displayID;
      GLFWmonitor *monitor = nullptr;
      get_glfw_monitor_from_sdl_display_id (sdl_monitor, &monitor);

      if (monitor && state.monitor_callback)
        state.monitor_callback (monitor, GLFW_DISCONNECTED);

      delete monitor;
    }
  else if (event.type == SDL_EVENT_MOUSE_MOTION)
    {
      if (window && window->cursor_pos_callback)
        window->cursor_pos_callback (window, event.motion.x, event.motion.y);
    }
  else if (event.type == SDL_EVENT_MOUSE_WHEEL)
    {
      if (window && window->scroll_callback)
        window->scroll_callback (window,
                                 static_cast<double> (event.wheel.x),
                                 static_cast<double> (event.wheel.y));
    }
  else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
      if (!window)
        return;
      int pressed = event.button.down ? GLFW_PRESS : GLFW_RELEASE;
      int button = (event.button.button < 6) ? sdl_to_glfw_btn[event.button.button] : -1;

      if (window->mouse_button_callback)
        window->mouse_button_callback (window, button, pressed, window->modifiers);
    }
  // TODO: drop
  // TODO: monitor
}

static void
drain_event_queue ()
{
  SDL_Event event;
  while (SDL_PollEvent (&event))
    {
      if (event.type != shim_wakeup_event_type)
        dispatch_event (event);
    }
}

extern "C" {

void
glfwPollEvents ()
{
  ASSERT_MAIN_THREAD_VOID ()
  glfw_init_events ();
  drain_event_queue ();
}

void
glfwWaitEvents ()
{
  ASSERT_MAIN_THREAD_VOID ()
  glfw_init_events ();

  SDL_Event event;
  if (SDL_WaitEvent (&event))
    {
      if (event.type != shim_wakeup_event_type)
        dispatch_event (event);

      drain_event_queue ();
    }
  else
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}

void
glfwWaitEventsTimeout (double timeout)
{
  ASSERT_MAIN_THREAD_VOID ();

  glfw_init_events ();

  if (std::isnan (timeout) || timeout < 0.0)
    {
      do_error_things (GLFW_INVALID_VALUE,
                       "Invalid timeout passed to glfwWaitEventsTimeout");
      return;
    }

  if (timeout == 0.0)
    {
      drain_event_queue ();
      return;
    }

  if (std::isinf (timeout))
    {
      glfwWaitEvents ();
      return;
    }

  auto duration = std::chrono::duration<double> (timeout);
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds> (duration);
  Sint32 timeout_ms = static_cast<Sint32> (duration_ms.count ());

  SDL_Event event;

  if (SDL_WaitEventTimeout (&event, timeout_ms))
    {
      if (event.type != shim_wakeup_event_type)
        dispatch_event (event);
      drain_event_queue ();
    }
}

void
glfwPostEmptyEvent ()
{
  run_on_main_sync ([&] ()
    {
      glfw_init_events ();
    });

  SDL_Event event{};
  event.type = shim_wakeup_event_type;

  if (!SDL_PushEvent (&event))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}

} // extern "C"
