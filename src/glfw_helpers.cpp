#include "glfw3.h"
#include "glfw_helpers.hpp"

#include <format>

// https://www.glfw.org/docs/latest/group__errors.html
std::optional<std::string_view>
err_to_string (int err)
{
  switch (err)
    {
    case GLFW_NO_ERROR:
      return "GLFW_NO_ERROR";
    case GLFW_NOT_INITIALIZED:
      return "GLFW_NOT_INITIALIZED";
    case GLFW_NO_CURRENT_CONTEXT:
      return "GLFW_NO_CURRENT_CONTEXT";
    case GLFW_INVALID_ENUM:
      return "GLFW_INVALID_ENUM";
    case GLFW_INVALID_VALUE:
      return "GLFW_INVALID_VALUE";
    case GLFW_OUT_OF_MEMORY:
      return "GLFW_OUT_OF_MEMORY";
    case GLFW_API_UNAVAILABLE:
      return "GLFW_API_UNAVAILABLE";
    case GLFW_VERSION_UNAVAILABLE:
      return "GLFW_VERSION_UNAVAILABLE";
    case GLFW_PLATFORM_ERROR:
      return "GLFW_PLATFORM_ERROR";
    case GLFW_FORMAT_UNAVAILABLE:
      return "GLFW_FORMAT_UNAVAILABLE";
    case GLFW_NO_WINDOW_CONTEXT:
      return "GLFW_NO_WINDOW_CONTEXT";
    case GLFW_CURSOR_UNAVAILABLE:
      return "GLFW_CURSOR_UNAVAILABLE";
    case GLFW_FEATURE_UNAVAILABLE:
      return "GLFW_FEATURE_UNAVAILABLE";
    case GLFW_FEATURE_UNIMPLEMENTED:
      return "GLFW_FEATURE_UNIMPLEMENTED";
    case GLFW_PLATFORM_UNAVAILABLE:
      return "GLFW_PLATFORM_UNAVAILABLE";
    default:
      return std::nullopt;
    }
}

void
do_error_things (int err, std::string_view ctx)
{
  std::string ctx_clarified;

  if (!err)
    return; // keep errors around until we actually get one, just like real GLFW

  thread_state.last_api_error = err;
  auto error_ctx = err_to_string (err);
  if (error_ctx)
    {
      ctx_clarified += *error_ctx;
      if (!ctx.empty())
        {
          ctx_clarified += ": ";
          ctx_clarified += ctx;
        }
      else
        ctx_clarified += ": No additional context for the error was provided.";
    }
  else
    {
      ctx_clarified += std::format ("Unknown integer value ({}), please file a bug.", err);
      if (!ctx.empty())
        {
          ctx_clarified += " Additional error context: ";
          ctx_clarified += ctx;
        }
      else
        ctx_clarified += " No additional context for the error was provided.";
    }

  thread_state.last_api_error_ctx = ctx_clarified;

  if (state.err_callback)
    state.err_callback (err, thread_state.last_api_error_ctx.c_str ());
}
