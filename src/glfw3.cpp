#include "glfw3.h"
#include <SDL3/SDL_init.h>

#include "config.h"

#include <string>
#include <format>
#include <optional>
#include <string_view>
#include <atomic>

struct ShimGlobalState {
  bool was_initialized;
  // TODO: enforce main-thread-only
  GLFWerrorfun err_callback;
};

// https://www.glfw.org/docs/3.3/intro_guide.html#error_handling
// errors are thread-local.
struct ShimThreadState {
  int last_api_error;
  std::optional<std::string> last_api_error_ctx;
};

static ShimGlobalState state;
static thread_local ShimThreadState thread_state;

// https://www.glfw.org/docs/latest/group__errors.html
static std::optional<std::string_view>
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

static void
do_error_things (int err, std::optional<std::string_view> ctx)
{
  std::string ctx_clarified;

  if (!err)
    return; // keep errors around until we actually get one, just like real GLFW

  thread_state.last_api_error = err;
  auto error_ctx = err_to_string (err);
  if (error_ctx)
    {
      ctx_clarified += *error_ctx;
      if (!ctx->empty())
        {
          ctx_clarified += ": ";
          ctx_clarified += *ctx;
        }
      else
        ctx_clarified += ": No additional context for the error was provided.";
    }
  else
    {
      ctx_clarified += std::format ("Unknown integer value ({}), please file a bug.", err);
      if (!ctx->empty())
        {
          ctx_clarified += " Additional error context: ";
          ctx_clarified += *ctx;
        }
      else
        ctx_clarified += " No additional context for the error was provided.";
    }

  thread_state.last_api_error_ctx = ctx_clarified;

  if (state.err_callback)
    state.err_callback (err, thread_state.last_api_error_ctx->c_str ());
}

extern "C" {

int
glfwInit ()
{
  int err = GLFW_NO_ERROR;
  const char *error_ctx = nullptr;

  if (state.was_initialized)
    return GLFW_TRUE;

  /* TODO */
  if (!SDL_Init (SDL_INIT_VIDEO))
    {
      error_ctx = SDL_GetError ();
      err = GLFW_PLATFORM_ERROR;
    }

  if (!err)
    {
      state.was_initialized = true;
      return GLFW_TRUE;
    }
  else
    {
      std::string_view ctx = error_ctx;
      do_error_things (err, ctx);

      glfwTerminate ();
      return GLFW_FALSE;
    }
}

void
glfwInitHint (int hint, int value)
{
  int err = GLFW_NO_ERROR;
  std::string ctx;

  if (state.was_initialized)
    return;

  switch (hint)
    {
    /* TODO */
    default:
      err = GLFW_INVALID_ENUM;
      ctx = "Unrecognized init hint";
    }

  do_error_things (err, ctx);
}

void
glfwTerminate ()
{
  SDL_Quit ();
  state.was_initialized = false;
}

int
glfwGetError (const char **description)
{
  int last_err_code = thread_state.last_api_error;
  thread_state.last_api_error = GLFW_NO_ERROR;

  if (description)
    {
      if (last_err_code)
        *description = thread_state.last_api_error_ctx->c_str ();
      else
        *description = nullptr;
    }

  return last_err_code;
}

GLFWerrorfun
glfwSetErrorCallback (GLFWerrorfun callback)
{
  GLFWerrorfun old_callback = state.err_callback;

  state.err_callback = callback;

  return old_callback;
}

void
glfwGetVersion (int *major, int *minor, int *rev)
{
  if (major)
    *major = GLFW_MAJOR_VERSION;
  if (minor)
    *minor = GLFW_MINOR_VERSION;
  if (rev)
    *rev = GLFW_PATCH_VERSION;
}

const char*
glfwGetVersionString ()
{
  // This (can) have some debug info that may be useful to users and devs
  return SHIM_VERSION_STRING;
}

} // extern "C"
