#include "helpers/glfw_helpers.hpp"
#include <cmath>
#include <chrono>

#include <SDL3/SDL_events.h>
#include "window/glfw_window.hpp"

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
  if (event.type >= SDL_EVENT_WINDOW_FIRST && event.type <= SDL_EVENT_WINDOW_LAST)
    glfw_window_dispatch_event (event.window);
}

static void
drain_event_queue()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
    {
      if (event.type != shim_wakeup_event_type)
        dispatch_event (event);
    }
}

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
  if (SDL_WaitEvent(&event)) {
    if (event.type != shim_wakeup_event_type)
      dispatch_event (event);

    drain_event_queue();
  } else
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

  if (SDL_WaitEventTimeout(&event, timeout_ms))
    {
      if (event.type != shim_wakeup_event_type)
          dispatch_event (event);
      drain_event_queue();
    }
}

void
glfwPostEmptyEvent ()
{
  run_on_main_sync([&]()
    {
      glfw_init_events ();
    });

  SDL_Event event{};
  event.type = shim_wakeup_event_type;

  if (!SDL_PushEvent(&event))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}
