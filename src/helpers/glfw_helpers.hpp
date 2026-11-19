#pragma once

#include "glfw3.h"

#include <SDL3/SDL_init.h>

#include <optional>
#include <string_view>
#include <string>

void
glfw_window_from_sdl_window (SDL_Window *sdl_window,
                             GLFWwindow **window);

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
  bool initialized;
  int selected_glfw_platform;
  bool failed_sdl_init;
  bool vulkan_loaded;
  GLFWerrorfun err_callback;
  GLFWvkproc vulkan_loader;
};

// https://www.glfw.org/docs/3.3/intro_guide.html#error_handling
// errors are thread-local.
struct ShimThreadState {
  int last_api_error;
  std::string last_api_error_ctx;
};

inline ShimGlobalState state;
inline thread_local ShimThreadState thread_state;

std::optional<std::string_view>
err_to_string (int err);

void
do_error_things (int err, std::string_view ctx);

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

int
assert_glfw_init ();

#define ASSERT_GLFW_INIT_INT()\
  if (!assert_glfw_init ())\
    return GLFW_FALSE;
