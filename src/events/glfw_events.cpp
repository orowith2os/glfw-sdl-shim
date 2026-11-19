#include "helpers/glfw_helpers.hpp"
#include <cmath>

#include <SDL3/SDL_events.h>
#include "window/glfw_window.hpp"

static Uint32 shim_wakeup_event_type = 0;

bool
glfw_init_events ()
{
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
  drain_event_queue ();
}

void
glfwWaitEvents ()
{
  ASSERT_MAIN_THREAD_VOID ()

  SDL_Event event;
  if (SDL_WaitEvent(&event)) {
    if (event.type != shim_wakeup_event_type) {
        return;
    }
    drain_event_queue();
  } else {
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
  }
}

void
glfwWaitEventsTimeout (double timeout)
{
  ASSERT_MAIN_THREAD_VOID ();

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

  double ms_double = timeout * 1000.0;
  Sint32 timeout_ms = (ms_double >= static_cast<double>(std::numeric_limits<Sint32>::max()))
    ? std::numeric_limits<Sint32>::max()
    : static_cast<Sint32>(ms_double);

  SDL_Event event;

  if (SDL_WaitEventTimeout(&event, timeout_ms))
    {
      if (event.type != shim_wakeup_event_type)
        {
          return; //TODO
        }
      drain_event_queue();
    }
}

void
glfwPostEmptyEvent ()
{
  if (shim_wakeup_event_type == 0)
    return;

  SDL_Event event{};
  event.type = shim_wakeup_event_type;

  if (!SDL_PushEvent(&event))
    do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
}
