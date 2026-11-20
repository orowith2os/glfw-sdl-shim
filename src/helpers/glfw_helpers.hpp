#pragma once

#include "glfw3.h"
#include "window/glfw_window.hpp"

#include <SDL3/SDL_init.h>

#include <optional>
#include <string_view>
#include <string>
#include "sdl_scancode.hpp"

SDL_Surface *
sdl_surface_from_glfw_image (const GLFWimage *image);

void
glfw_window_from_sdl_window (SDL_Window *sdl_window,
                             GLFWwindow **window);

GLFWwindow *
glfw_window_from_sdl_window_id (SDL_WindowID id);

template <typename Func>
[[maybe_unused]]
static bool
run_on_main_sync (Func&& func)
{
  using FuncType = std::remove_reference_t<Func>;
  return SDL_RunOnMainThread (
    [](void *userdata) {
      (*static_cast<FuncType *>(userdata)) ();
    },
    &func,
    true
  );
}

struct ShimGlobalState {
  bool initialized = false;
  bool vulkan_loaded = false;

  int selected_glfw_platform = GLFW_ANY_PLATFORM;
  GLFWerrorfun err_callback = nullptr;
  GLFWvkproc vulkan_loader = nullptr;
  uint64_t timer_start_ms = 0;
  uint64_t base_time_ms = 0;
  GLFWjoystickfun joystick_callback = nullptr;
  GLFWwindow *focused_window;
};

// https://www.glfw.org/docs/3.3/intro_guide.html#error_handling
// errors are thread-local.
struct ShimThreadState {
  int last_api_error = GLFW_NO_ERROR;
  std::string last_api_error_ctx;
};

inline ShimGlobalState state;
inline thread_local ShimThreadState thread_state;

std::optional<std::string_view>
err_to_string (int err);

void
do_error_things (int err, std::string_view ctx);

[[nodiscard]] constexpr int
translate_sdl_keymod_to_glfw (GLFWwindow *window, SDL_Keymod sdl_keymods) noexcept
{
  int mods = 0;

  if (sdl_keymods & SDL_KMOD_SHIFT)
    mods |= GLFW_MOD_SHIFT;
  if (sdl_keymods & SDL_KMOD_CTRL)
    mods |= GLFW_MOD_CONTROL;
  if (sdl_keymods & SDL_KMOD_ALT)
    mods |= GLFW_MOD_ALT;
  if (sdl_keymods & SDL_KMOD_GUI)
    mods |= GLFW_MOD_SUPER;
  if (sdl_keymods & SDL_KMOD_CAPS)
    mods |= GLFW_MOD_CAPS_LOCK;
  if (sdl_keymods & SDL_KMOD_NUM)
    mods |= GLFW_MOD_NUM_LOCK;

  if (!window->lock_key_mods)
    mods &= ~(GLFW_MOD_CAPS_LOCK | GLFW_MOD_NUM_LOCK);

  return mods;
}

bool
glfw_init_events ();

int
assert_main_thread ();

#define ASSERT_MAIN_THREAD_INT()\
  if (!assert_main_thread ())\
    return GLFW_FALSE;

// returns int, so don't use otherwise
#define ASSERT_MAIN_THREAD_VOID()\
  if (!assert_main_thread ())\
    return;

#define ASSERT_MAIN_THREAD_PTR()\
  if (!assert_main_thread ())\
    return nullptr;

#define ASSERT_MAIN_THREAD(val)\
  if (!assert_main_thread ())\
    return val;

int
assert_glfw_init ();

#define ASSERT_GLFW_INIT_INT()\
  if (!assert_glfw_init ())\
    return GLFW_FALSE;

// returns int, so don't use otherwise
#define ASSERT_GLFW_INIT_VOID()\
  if (!assert_glfw_init ())\
    return;

#define ASSERT_GLFW_INIT_PTR()\
  if (!assert_glfw_init ())\
    return nullptr;

#define ASSERT_GLFW_INIT(val)\
  if (!assert_glfw_init ())\
    return val;

#define ASSERT_VALID_WIN(win, val)\
  if (!win && !win->sdl_window)\
    return val;

#define ASSERT_VALID_WIN_VOID(win)\
  if (!win && !win->sdl_window)\
    return;
