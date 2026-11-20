#include "glfw3.h"

#include <array>
#include <cstdint>
#include <utility>

namespace detail
{
  inline constexpr std::pair<SDL_Scancode, int> key_mappings[] = {
    // Letters
    { SDL_SCANCODE_A, GLFW_KEY_A },
    { SDL_SCANCODE_B, GLFW_KEY_B },
    { SDL_SCANCODE_C, GLFW_KEY_C },
    { SDL_SCANCODE_D, GLFW_KEY_D },
    { SDL_SCANCODE_E, GLFW_KEY_E },
    { SDL_SCANCODE_F, GLFW_KEY_F },
    { SDL_SCANCODE_G, GLFW_KEY_G },
    { SDL_SCANCODE_H, GLFW_KEY_H },
    { SDL_SCANCODE_I, GLFW_KEY_I },
    { SDL_SCANCODE_J, GLFW_KEY_J },
    { SDL_SCANCODE_K, GLFW_KEY_K },
    { SDL_SCANCODE_L, GLFW_KEY_L },
    { SDL_SCANCODE_M, GLFW_KEY_M },
    { SDL_SCANCODE_N, GLFW_KEY_N },
    { SDL_SCANCODE_O, GLFW_KEY_O },
    { SDL_SCANCODE_P, GLFW_KEY_P },
    { SDL_SCANCODE_Q, GLFW_KEY_Q },
    { SDL_SCANCODE_R, GLFW_KEY_R },
    { SDL_SCANCODE_S, GLFW_KEY_S },
    { SDL_SCANCODE_T, GLFW_KEY_T },
    { SDL_SCANCODE_U, GLFW_KEY_U },
    { SDL_SCANCODE_V, GLFW_KEY_V },
    { SDL_SCANCODE_W, GLFW_KEY_W },
    { SDL_SCANCODE_X, GLFW_KEY_X },
    { SDL_SCANCODE_Y, GLFW_KEY_Y },
    { SDL_SCANCODE_Z, GLFW_KEY_Z },

    // Top-row Numbers
    { SDL_SCANCODE_1, GLFW_KEY_1 },
    { SDL_SCANCODE_2, GLFW_KEY_2 },
    { SDL_SCANCODE_3, GLFW_KEY_3 },
    { SDL_SCANCODE_4, GLFW_KEY_4 },
    { SDL_SCANCODE_5, GLFW_KEY_5 },
    { SDL_SCANCODE_6, GLFW_KEY_6 },
    { SDL_SCANCODE_7, GLFW_KEY_7 },
    { SDL_SCANCODE_8, GLFW_KEY_8 },
    { SDL_SCANCODE_9, GLFW_KEY_9 },
    { SDL_SCANCODE_0, GLFW_KEY_0 },

    // Control & Whitespace
    { SDL_SCANCODE_RETURN, GLFW_KEY_ENTER },
    { SDL_SCANCODE_ESCAPE, GLFW_KEY_ESCAPE },
    { SDL_SCANCODE_BACKSPACE, GLFW_KEY_BACKSPACE },
    { SDL_SCANCODE_TAB, GLFW_KEY_TAB },
    { SDL_SCANCODE_SPACE, GLFW_KEY_SPACE },

    // Punctuation & Symbols
    { SDL_SCANCODE_MINUS, GLFW_KEY_MINUS },
    { SDL_SCANCODE_EQUALS, GLFW_KEY_EQUAL },
    { SDL_SCANCODE_LEFTBRACKET, GLFW_KEY_LEFT_BRACKET },
    { SDL_SCANCODE_RIGHTBRACKET, GLFW_KEY_RIGHT_BRACKET },
    { SDL_SCANCODE_BACKSLASH, GLFW_KEY_BACKSLASH },
    { SDL_SCANCODE_SEMICOLON, GLFW_KEY_SEMICOLON },
    { SDL_SCANCODE_APOSTROPHE, GLFW_KEY_APOSTROPHE },
    { SDL_SCANCODE_GRAVE, GLFW_KEY_GRAVE_ACCENT },
    { SDL_SCANCODE_COMMA, GLFW_KEY_COMMA },
    { SDL_SCANCODE_PERIOD, GLFW_KEY_PERIOD },
    { SDL_SCANCODE_SLASH, GLFW_KEY_SLASH },

    // Caps Lock & Function Keys
    { SDL_SCANCODE_CAPSLOCK, GLFW_KEY_CAPS_LOCK },
    { SDL_SCANCODE_F1, GLFW_KEY_F1 },
    { SDL_SCANCODE_F2, GLFW_KEY_F2 },
    { SDL_SCANCODE_F3, GLFW_KEY_F3 },
    { SDL_SCANCODE_F4, GLFW_KEY_F4 },
    { SDL_SCANCODE_F5, GLFW_KEY_F5 },
    { SDL_SCANCODE_F6, GLFW_KEY_F6 },
    { SDL_SCANCODE_F7, GLFW_KEY_F7 },
    { SDL_SCANCODE_F8, GLFW_KEY_F8 },
    { SDL_SCANCODE_F9, GLFW_KEY_F9 },
    { SDL_SCANCODE_F10, GLFW_KEY_F10 },
    { SDL_SCANCODE_F11, GLFW_KEY_F11 },
    { SDL_SCANCODE_F12, GLFW_KEY_F12 },
    { SDL_SCANCODE_F13, GLFW_KEY_F13 },
    { SDL_SCANCODE_F14, GLFW_KEY_F14 },
    { SDL_SCANCODE_F15, GLFW_KEY_F15 },
    { SDL_SCANCODE_F16, GLFW_KEY_F16 },
    { SDL_SCANCODE_F17, GLFW_KEY_F17 },
    { SDL_SCANCODE_F18, GLFW_KEY_F18 },
    { SDL_SCANCODE_F19, GLFW_KEY_F19 },
    { SDL_SCANCODE_F20, GLFW_KEY_F20 },
    { SDL_SCANCODE_F21, GLFW_KEY_F21 },
    { SDL_SCANCODE_F22, GLFW_KEY_F22 },
    { SDL_SCANCODE_F23, GLFW_KEY_F23 },
    { SDL_SCANCODE_F24, GLFW_KEY_F24 },

    // Navigation & Editing
    { SDL_SCANCODE_PRINTSCREEN, GLFW_KEY_PRINT_SCREEN },
    { SDL_SCANCODE_SCROLLLOCK, GLFW_KEY_SCROLL_LOCK },
    { SDL_SCANCODE_PAUSE, GLFW_KEY_PAUSE },
    { SDL_SCANCODE_INSERT, GLFW_KEY_INSERT },
    { SDL_SCANCODE_HOME, GLFW_KEY_HOME },
    { SDL_SCANCODE_PAGEUP, GLFW_KEY_PAGE_UP },
    { SDL_SCANCODE_DELETE, GLFW_KEY_DELETE },
    { SDL_SCANCODE_END, GLFW_KEY_END },
    { SDL_SCANCODE_PAGEDOWN, GLFW_KEY_PAGE_DOWN },
    { SDL_SCANCODE_RIGHT, GLFW_KEY_RIGHT },
    { SDL_SCANCODE_LEFT, GLFW_KEY_LEFT },
    { SDL_SCANCODE_DOWN, GLFW_KEY_DOWN },
    { SDL_SCANCODE_UP, GLFW_KEY_UP },

    // Keypad / Numpad
    { SDL_SCANCODE_NUMLOCKCLEAR, GLFW_KEY_NUM_LOCK },
    { SDL_SCANCODE_KP_DIVIDE, GLFW_KEY_KP_DIVIDE },
    { SDL_SCANCODE_KP_MULTIPLY, GLFW_KEY_KP_MULTIPLY },
    { SDL_SCANCODE_KP_MINUS, GLFW_KEY_KP_SUBTRACT },
    { SDL_SCANCODE_KP_PLUS, GLFW_KEY_KP_ADD },
    { SDL_SCANCODE_KP_ENTER, GLFW_KEY_KP_ENTER },
    { SDL_SCANCODE_KP_1, GLFW_KEY_KP_1 },
    { SDL_SCANCODE_KP_2, GLFW_KEY_KP_2 },
    { SDL_SCANCODE_KP_3, GLFW_KEY_KP_3 },
    { SDL_SCANCODE_KP_4, GLFW_KEY_KP_4 },
    { SDL_SCANCODE_KP_5, GLFW_KEY_KP_5 },
    { SDL_SCANCODE_KP_6, GLFW_KEY_KP_6 },
    { SDL_SCANCODE_KP_7, GLFW_KEY_KP_7 },
    { SDL_SCANCODE_KP_8, GLFW_KEY_KP_8 },
    { SDL_SCANCODE_KP_9, GLFW_KEY_KP_9 },
    { SDL_SCANCODE_KP_0, GLFW_KEY_KP_0 },
    { SDL_SCANCODE_KP_PERIOD, GLFW_KEY_KP_DECIMAL },
    { SDL_SCANCODE_KP_EQUALS, GLFW_KEY_KP_EQUAL },

    // International & Application Keys
    { SDL_SCANCODE_NONUSBACKSLASH, GLFW_KEY_WORLD_2 },
    { SDL_SCANCODE_APPLICATION, GLFW_KEY_MENU },

    // Modifier Keys
    { SDL_SCANCODE_LCTRL, GLFW_KEY_LEFT_CONTROL },
    { SDL_SCANCODE_LSHIFT, GLFW_KEY_LEFT_SHIFT },
    { SDL_SCANCODE_LALT, GLFW_KEY_LEFT_ALT },
    { SDL_SCANCODE_LGUI, GLFW_KEY_LEFT_SUPER },
    { SDL_SCANCODE_RCTRL, GLFW_KEY_RIGHT_CONTROL },
    { SDL_SCANCODE_RSHIFT, GLFW_KEY_RIGHT_SHIFT },
    { SDL_SCANCODE_RALT, GLFW_KEY_RIGHT_ALT },
    { SDL_SCANCODE_RGUI, GLFW_KEY_RIGHT_SUPER },
  };

  // Trimmed to the highest mapped scancode (SDL_SCANCODE_RGUI = 231) instead of 512
  inline constexpr std::size_t max_mapped_scancode = [] () consteval {
    std::size_t max_sc = 0;
    for (const auto& [sc, _] : key_mappings)
      if (static_cast<std::size_t> (sc) > max_sc)
        max_sc = static_cast<std::size_t> (sc);
    return max_sc;
  }();

  inline constexpr auto sdl_to_glfw_table = [] () consteval
    {
      std::array<std::int16_t, max_mapped_scancode + 1> map {};
      map.fill (GLFW_KEY_UNKNOWN);
      for (const auto& [sc, glfw] : key_mappings)
        map[sc] = static_cast<std::int16_t> (glfw);
      return map;
    }();

  inline constexpr auto glfw_to_sdl_table = [] () consteval
    {
      std::array<std::uint16_t, GLFW_KEY_LAST + 1> map {};
      map.fill (SDL_SCANCODE_UNKNOWN);
      for (const auto& [sc, glfw] : key_mappings)
        map[glfw] = static_cast<std::uint16_t> (sc);
      return map;
    }();
}

[[nodiscard]] constexpr int
translate_sdl_scancode_to_glfw (SDL_Scancode scancode) noexcept
{
  const auto idx = static_cast<std::size_t> (scancode);
  if (idx >= detail::sdl_to_glfw_table.size ())
    return GLFW_KEY_UNKNOWN;

  return detail::sdl_to_glfw_table[idx];
}

[[nodiscard]] constexpr SDL_Scancode
translate_glfw_key_to_sdl_scancode (int key) noexcept
{
  const auto idx = static_cast<std::size_t> (key);
  if (idx >= detail::glfw_to_sdl_table.size ())
    return SDL_SCANCODE_UNKNOWN;

  return static_cast<SDL_Scancode> (detail::glfw_to_sdl_table[idx]);
}
