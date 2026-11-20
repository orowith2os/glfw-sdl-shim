#include "glfw_monitor.hpp"
#include "helpers/glfw_helpers.hpp"

static int
countBits (uint32_t mask)
{
    int n = 0;
    while (mask)
      {
        n += mask & 1;
        mask >>= 1;
      }
    return n;
}

// copied from GLFW
static void
split_bpp (int bpp, int *red, int *green, int *blue)
{
    int delta;

    // We assume that by 32 the user really meant 24
    if (bpp == 32)
        bpp = 24;

    // Convert "bits per pixel" to red, green & blue sizes

    *red = *green = *blue = bpp / 3;
    delta = bpp - (*red * 3);
    if (delta >= 1)
        *green = *green + 1;

    if (delta == 2)
        *red = *red + 1;
}

GLFWvidmode
sdl_display_mode_to_vidmode (const SDL_DisplayMode *m)
{
    GLFWvidmode out{};
    out.width       = m->w;
    out.height      = m->h;
    out.refreshRate = (int) (m->refresh_rate + 0.5f); // float in SDL3

    const SDL_PixelFormatDetails *d = SDL_GetPixelFormatDetails (m->format);
    if (d && d->Rmask && d->Gmask && d->Bmask)
      {
        out.redBits   = countBits (d->Rmask);
        out.greenBits = countBits (d->Gmask);
        out.blueBits  = countBits (d->Bmask);
      }
    else
      {
        int bpp = d ? d->bits_per_pixel : 24;
        split_bpp (bpp, &out.redBits, &out.greenBits, &out.blueBits);
      }

    return out;
}

bool
glfw_monitor_update_current_mode (GLFWmonitor *monitor)
{
  const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode (monitor->sdl_displayid);
  if (!mode)
    return false;

  monitor->current_mode = sdl_display_mode_to_vidmode (mode);
  return true;
}

extern "C" {

const GLFWvidmode *
glfwGetVideoModes (GLFWmonitor *monitor, int *count)
{
  if (count)
    *count = 0;
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)
  ASSERT_VALID_MON (monitor, nullptr)
  if (!count)
    return nullptr;

  if (!glfw_monitor_update_current_mode (monitor))
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      return nullptr;
    }

  *count = 1;
  return &monitor->current_mode;
}

const GLFWvidmode *
glfwGetVideoMode (GLFWmonitor *monitor)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)
  ASSERT_VALID_MON (monitor, nullptr)

  if (!glfw_monitor_update_current_mode (monitor))
    {
      do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
      return nullptr;
    }

  return &monitor->current_mode;
}

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

  SDL_Rect bounds{};
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

  SDL_Rect bounds{};
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
