#include "helpers/glfw_helpers.hpp"
#include <SDL3/SDL_timer.h>
#include <cmath>
#include <chrono>

constexpr double max_seconds =
  static_cast<double> (std::numeric_limits<uint64_t>::max ()) / 1000.0;

static uint64_t
seconds_to_ms (double time_s)
{
  if (time_s >= max_seconds)
    return std::numeric_limits<uint64_t>::max ();

  // Why did nobody teach me about this before...
  using DoubleSeconds = std::chrono::duration<double>;
  using UInt64Ms = std::chrono::duration<uint64_t, std::milli>;

  auto ms_duration = std::chrono::round<UInt64Ms>(DoubleSeconds(time_s));
  return ms_duration.count();
}

extern "C" {

// This can be called from any thread.
double
glfwGetTime ()
{
  double time = 0.0;
  if (!state.initialized)
    return 0.0;

  run_on_main_sync ([&]()
    {
      const uint64_t elapsed = SDL_GetTicks () - state.base_time_ms;
      time = static_cast<double> (state.timer_start_ms + elapsed) / 1000.0;
    });

  return time;
}

// This can be called from any thread.
// This sets the starting point for the timer, not necessarily the offset.
void
glfwSetTime (double time_s)
{
  ASSERT_GLFW_INIT_VOID ()

  if (!std::isfinite (time_s) || time_s < 0.0 || time_s >= max_seconds)
    {
      do_error_things (GLFW_INVALID_VALUE, "Invalid value passed to glfwSetTime.");
      return;
    }

uint64_t time = seconds_to_ms (time_s);

  run_on_main_sync ([&]()
    {
      state.base_time_ms = SDL_GetTicks ();
      state.timer_start_ms = time;
    });
}

uint64_t
glfwGetTimerValue ()
{
  if (!state.initialized)
    return 0;

  return SDL_GetPerformanceCounter ();
}

uint64_t
glfwGetTimerFrequency ()
{
  if (!state.initialized)
    return 0;

  return SDL_GetPerformanceFrequency ();
}

} // extern "C"
