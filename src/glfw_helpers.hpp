#pragma once

#include <optional>
#include <string_view>
#include <string>

struct ShimGlobalState {
  bool initialized;
  int selected_glfw_platform;
  bool failed_sdl_init;
  bool vulkan_loaded;
  GLFWerrorfun err_callback;
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