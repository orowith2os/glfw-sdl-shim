#include "config.h"
#include "helpers/glfw_helpers.hpp"
#include "helpers/native/scancodes_linux_flipped.h"

#include <SDL3/SDL_platform.h>
#include <format>
#include <string>

void
glfw_mouse_button_dispatch_event (GLFWwindow *window, SDL_MouseButtonEvent event)
{
  ASSERT_VALID_WIN_VOID (window)

  int pressed = event.down ? GLFW_PRESS : GLFW_RELEASE;
  int button = (event.button < 6) ? sdl_to_glfw_btn[event.button] : -1;
  if (button < 0)
    goto callback;

  if (pressed == GLFW_RELEASE && window->mouse_buttons[button] == GLFW_RELEASE)
    return;

  if (pressed == GLFW_RELEASE && window->sticky_mouse)
    window->mouse_buttons[button] = GAY_ORO_GLFW_STICK;
  else
    window->mouse_buttons[button] = button;

callback:
  if (window->mouse_button_callback)
    window->mouse_button_callback (window, button, pressed, window->modifiers);
}

void
glfw_key_dispatch_event (GLFWwindow *window, SDL_KeyboardEvent event)
{
  ASSERT_VALID_WIN_VOID (window)

  const int key = translate_sdl_scancode_to_glfw (event.scancode);
  window->modifiers = translate_sdl_keymod_to_glfw (window, event.mod);
  int action = event.down
                 ? (event.repeat ? GLFW_REPEAT : GLFW_PRESS)
                 : GLFW_RELEASE;

  if (key >= 0 && key <= GLFW_KEY_LAST)
    {
      if (action == GLFW_RELEASE && window->keys[key] == GLFW_RELEASE)
        return;

      if (action == GLFW_RELEASE && window->sticky_keys)
        window->keys[key] = GAY_ORO_GLFW_STICK;
      else
        window->keys[key] = (char) (action == GLFW_REPEAT ? GLFW_PRESS : action);
    }

  if (window->key_callback)
    window->key_callback (window,
                          translate_sdl_scancode_to_glfw (event.scancode),
                          event.raw, action,
                          window->modifiers);
}

extern "C" {

int
glfwGetMouseButton (GLFWwindow *window, int button)
{
  ASSERT_GLFW_INIT (GLFW_RELEASE)
  ASSERT_VALID_WIN (window, GLFW_RELEASE)
  if (button < GLFW_MOUSE_BUTTON_1 || button > GLFW_MOUSE_BUTTON_LAST)
    {
      std::string ctx = std::format ("Invalid mouse button {}", button);
      do_error_things (GLFW_INVALID_ENUM, ctx);
      return GLFW_RELEASE;
    }

  if (window->mouse_buttons[button] == GAY_ORO_GLFW_STICK)
    {
      window->mouse_buttons[button] = GLFW_RELEASE;
      return GLFW_PRESS;
    }

  return window->mouse_buttons[button];
}

int
glfwGetKey (GLFWwindow *window, int key)
{
  ASSERT_GLFW_INIT (GLFW_RELEASE)
  ASSERT_VALID_WIN (window, GLFW_RELEASE)

  if (key < GLFW_KEY_SPACE || key > GLFW_KEY_LAST)
    {
      std::string ctx = std::format ("Invalid key {}", key);
      do_error_things (GLFW_INVALID_ENUM, ctx);
      return GLFW_RELEASE;
    }

  if (window->keys[key] == GAY_ORO_GLFW_STICK)
    {
      window->keys[key] = GLFW_RELEASE;
      return GLFW_PRESS;
    }

  return window->keys[key];
}

int
glfwGetKeyScancode (int key)
{
  ASSERT_GLFW_INIT (-1)

  const char *platform = SDL_GetPlatform ();
  if (SDL_strcmp (platform, "Linux") == 0)
    return sdl_scancode_to_linux (translate_glfw_key_to_sdl_scancode (key));
  else
    return -1;
}

const char *
glfwGetKeyName (int key, int scancode)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)
  SDL_Scancode sdl_scancode;

  const char *platform = SDL_GetPlatform ();
  if (SDL_strcmp (platform, "Linux") == 0)
    {
      if (key == GLFW_KEY_UNKNOWN)
        sdl_scancode = linux_to_sdl_scancode (scancode);
      else
        sdl_scancode = translate_glfw_key_to_sdl_scancode (key);
    }
  else
    return nullptr;

  int glfw_key = translate_sdl_scancode_to_glfw (sdl_scancode);
  if (!is_printable_glfw_key (glfw_key))
    return nullptr;

  const SDL_Keycode keycode = SDL_GetKeyFromScancode (sdl_scancode, SDL_KMOD_NONE, false);
  if (keycode == SDLK_UNKNOWN)
    return nullptr;

  const char *name = SDL_GetKeyName (keycode);
  return (name && *name != '\0') ? name : nullptr;
}

}
