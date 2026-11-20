#include "glfw_monitor.hpp"
#include "helpers/glfw_helpers.hpp"

extern "C" {

GLFWmonitor **
glfwGetMonitors (int *count)
{
  if (count)
    *count = 0;
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)

  if (count)
    *count = state.monitors.size ();
  return state.monitors.data ();
}

// As per GLFW, first monitor [that would be] returned by glfwGetMonitors.
GLFWmonitor *
glfwGetPrimaryMonitor ()
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)

  return (state.monitors.size () > 0) ? state.monitors[0] : nullptr;
}

void
glfwGetMonitorPos (GLFWmonitor *monitor,
                   int *xpos, int *ypos)
{
  if (xpos)
    *xpos = 0;
  if (ypos)
    *ypos = 0;
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_VALID_MON_VOID (monitor)
  ASSERT_MAIN_THREAD_VOID ()

  SDL_Rect bounds;
  if (SDL_GetDisplayBounds (monitor->sdl_displayid, &bounds))
    {
      if (xpos)
        *xpos = bounds.x;
      if (ypos)
        *ypos = bounds.y;
    }
}


void
glfwGetMonitorWorkarea (GLFWmonitor *monitor,
                        int *xpos, int *ypos,
                        int *width, int *height)
{
  if (xpos)
    *xpos = 0;
  if (ypos)
    *ypos = 0;
  if (width)
    *width = 0;
  if (height)
    *height = 0;
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_VALID_MON_VOID (monitor)
  ASSERT_MAIN_THREAD_VOID ()

  SDL_Rect bounds;
  if (SDL_GetDisplayUsableBounds (monitor->sdl_displayid, &bounds))
    {
      if (xpos)
        *xpos = bounds.x;
      if (ypos)
        *ypos = bounds.y;
      if (width)
        *width = bounds.w;
      if (height)
        *height = bounds.h;
    }
}

void
glfwGetMonitorPhysicalSize (GLFWmonitor *monitor,
                            int *widthMM, int *heightMM)
{
  if (widthMM)
    *widthMM = 0;
  if (heightMM)
    *heightMM = 0;
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_VALID_MON_VOID (monitor)
  ASSERT_MAIN_THREAD_VOID ()

  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode (monitor->sdl_displayid);
  if (!mode)
    return;

  // GLFW doesn't use scale here, I think? But we might as well.
  float scale = SDL_GetDisplayContentScale (monitor->sdl_displayid);
  if (scale <= 0.0f)
    scale = 1.0f;

  float dpi = 96.0f * scale; // GLFW also assumes a default 96 DPI

  if (widthMM)
    *widthMM  = (int) (mode->w * 25.4f / dpi); // GLFW also does 25.4f.
  if (heightMM)
    *heightMM = (int) (mode->h * 25.4f / dpi);
}

void
glfwGetMonitorContentScale (GLFWmonitor *monitor,
                            float *xscale, float *yscale)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_VALID_MON_VOID (monitor)
  ASSERT_MAIN_THREAD_VOID ()

  float scale = SDL_GetDisplayContentScale (monitor->sdl_displayid);

  if (xscale)
    *xscale = scale;
  if (yscale)
    *yscale = scale;
}

const char *
glfwGetMonitorName (GLFWmonitor *monitor)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_VALID_MON (monitor, nullptr)
  ASSERT_MAIN_THREAD (nullptr)

  return SDL_GetDisplayName (monitor->sdl_displayid);
}

// This can be called from any thread.
void
glfwSetMonitorUserPointer (GLFWmonitor *monitor, void *pointer)
{
  ASSERT_GLFW_INIT_VOID ()
  ASSERT_VALID_MON_VOID (monitor)

  run_on_main_sync ([&]()
    {
      monitor->user_pointer = pointer;
    });
}

// This can be called from any thread.
void *
glfwGetMonitorUserPointer (GLFWmonitor *monitor)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_VALID_MON (monitor, nullptr)
  void *ptr = nullptr;

  run_on_main_sync ([&]()
    {
      ptr = monitor->user_pointer;
    });

  return ptr;
}

} // extern "C"
