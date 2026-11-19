#include <utility>
#include <array>

[[nodiscard]] constexpr int
translate_sdl_scancode_to_glfw (SDL_Scancode scancode) noexcept
{
  constexpr auto table = [] () consteval {
    std::array<int, SDL_SCANCODE_COUNT> map {};
    map.fill (GLFW_KEY_UNKNOWN);

    // Letters
    map[SDL_SCANCODE_A] = GLFW_KEY_A;
    map[SDL_SCANCODE_B] = GLFW_KEY_B;
    map[SDL_SCANCODE_C] = GLFW_KEY_C;
    map[SDL_SCANCODE_D] = GLFW_KEY_D;
    map[SDL_SCANCODE_E] = GLFW_KEY_E;
    map[SDL_SCANCODE_F] = GLFW_KEY_F;
    map[SDL_SCANCODE_G] = GLFW_KEY_G;
    map[SDL_SCANCODE_H] = GLFW_KEY_H;
    map[SDL_SCANCODE_I] = GLFW_KEY_I;
    map[SDL_SCANCODE_J] = GLFW_KEY_J;
    map[SDL_SCANCODE_K] = GLFW_KEY_K;
    map[SDL_SCANCODE_L] = GLFW_KEY_L;
    map[SDL_SCANCODE_M] = GLFW_KEY_M;
    map[SDL_SCANCODE_N] = GLFW_KEY_N;
    map[SDL_SCANCODE_O] = GLFW_KEY_O;
    map[SDL_SCANCODE_P] = GLFW_KEY_P;
    map[SDL_SCANCODE_Q] = GLFW_KEY_Q;
    map[SDL_SCANCODE_R] = GLFW_KEY_R;
    map[SDL_SCANCODE_S] = GLFW_KEY_S;
    map[SDL_SCANCODE_T] = GLFW_KEY_T;
    map[SDL_SCANCODE_U] = GLFW_KEY_U;
    map[SDL_SCANCODE_V] = GLFW_KEY_V;
    map[SDL_SCANCODE_W] = GLFW_KEY_W;
    map[SDL_SCANCODE_X] = GLFW_KEY_X;
    map[SDL_SCANCODE_Y] = GLFW_KEY_Y;
    map[SDL_SCANCODE_Z] = GLFW_KEY_Z;

    // Top-row Numbers
    map[SDL_SCANCODE_1] = GLFW_KEY_1;
    map[SDL_SCANCODE_2] = GLFW_KEY_2;
    map[SDL_SCANCODE_3] = GLFW_KEY_3;
    map[SDL_SCANCODE_4] = GLFW_KEY_4;
    map[SDL_SCANCODE_5] = GLFW_KEY_5;
    map[SDL_SCANCODE_6] = GLFW_KEY_6;
    map[SDL_SCANCODE_7] = GLFW_KEY_7;
    map[SDL_SCANCODE_8] = GLFW_KEY_8;
    map[SDL_SCANCODE_9] = GLFW_KEY_9;
    map[SDL_SCANCODE_0] = GLFW_KEY_0;

    // Control & Whitespace
    map[SDL_SCANCODE_RETURN]    = GLFW_KEY_ENTER;
    map[SDL_SCANCODE_ESCAPE]    = GLFW_KEY_ESCAPE;
    map[SDL_SCANCODE_BACKSPACE] = GLFW_KEY_BACKSPACE;
    map[SDL_SCANCODE_TAB]       = GLFW_KEY_TAB;
    map[SDL_SCANCODE_SPACE]     = GLFW_KEY_SPACE;

    // Punctuation & Symbols
    map[SDL_SCANCODE_MINUS]        = GLFW_KEY_MINUS;
    map[SDL_SCANCODE_EQUALS]       = GLFW_KEY_EQUAL;
    map[SDL_SCANCODE_LEFTBRACKET]  = GLFW_KEY_LEFT_BRACKET;
    map[SDL_SCANCODE_RIGHTBRACKET] = GLFW_KEY_RIGHT_BRACKET;
    map[SDL_SCANCODE_BACKSLASH]    = GLFW_KEY_BACKSLASH;
    map[SDL_SCANCODE_SEMICOLON]    = GLFW_KEY_SEMICOLON;
    map[SDL_SCANCODE_APOSTROPHE]   = GLFW_KEY_APOSTROPHE;
    map[SDL_SCANCODE_GRAVE]        = GLFW_KEY_GRAVE_ACCENT;
    map[SDL_SCANCODE_COMMA]        = GLFW_KEY_COMMA;
    map[SDL_SCANCODE_PERIOD]       = GLFW_KEY_PERIOD;
    map[SDL_SCANCODE_SLASH]        = GLFW_KEY_SLASH;

    // Caps Lock & Function Keys
    map[SDL_SCANCODE_CAPSLOCK] = GLFW_KEY_CAPS_LOCK;
    map[SDL_SCANCODE_F1]       = GLFW_KEY_F1;
    map[SDL_SCANCODE_F2]       = GLFW_KEY_F2;
    map[SDL_SCANCODE_F3]       = GLFW_KEY_F3;
    map[SDL_SCANCODE_F4]       = GLFW_KEY_F4;
    map[SDL_SCANCODE_F5]       = GLFW_KEY_F5;
    map[SDL_SCANCODE_F6]       = GLFW_KEY_F6;
    map[SDL_SCANCODE_F7]       = GLFW_KEY_F7;
    map[SDL_SCANCODE_F8]       = GLFW_KEY_F8;
    map[SDL_SCANCODE_F9]       = GLFW_KEY_F9;
    map[SDL_SCANCODE_F10]      = GLFW_KEY_F10;
    map[SDL_SCANCODE_F11]      = GLFW_KEY_F11;
    map[SDL_SCANCODE_F12]      = GLFW_KEY_F12;
    map[SDL_SCANCODE_F13]      = GLFW_KEY_F13;
    map[SDL_SCANCODE_F14]      = GLFW_KEY_F14;
    map[SDL_SCANCODE_F15]      = GLFW_KEY_F15;
    map[SDL_SCANCODE_F16]      = GLFW_KEY_F16;
    map[SDL_SCANCODE_F17]      = GLFW_KEY_F17;
    map[SDL_SCANCODE_F18]      = GLFW_KEY_F18;
    map[SDL_SCANCODE_F19]      = GLFW_KEY_F19;
    map[SDL_SCANCODE_F20]      = GLFW_KEY_F20;
    map[SDL_SCANCODE_F21]      = GLFW_KEY_F21;
    map[SDL_SCANCODE_F22]      = GLFW_KEY_F22;
    map[SDL_SCANCODE_F23]      = GLFW_KEY_F23;
    map[SDL_SCANCODE_F24]      = GLFW_KEY_F24;

    // Navigation & Editing
    map[SDL_SCANCODE_PRINTSCREEN] = GLFW_KEY_PRINT_SCREEN;
    map[SDL_SCANCODE_SCROLLLOCK]  = GLFW_KEY_SCROLL_LOCK;
    map[SDL_SCANCODE_PAUSE]       = GLFW_KEY_PAUSE;
    map[SDL_SCANCODE_INSERT]      = GLFW_KEY_INSERT;
    map[SDL_SCANCODE_HOME]        = GLFW_KEY_HOME;
    map[SDL_SCANCODE_PAGEUP]      = GLFW_KEY_PAGE_UP;
    map[SDL_SCANCODE_DELETE]      = GLFW_KEY_DELETE;
    map[SDL_SCANCODE_END]         = GLFW_KEY_END;
    map[SDL_SCANCODE_PAGEDOWN]    = GLFW_KEY_PAGE_DOWN;
    map[SDL_SCANCODE_RIGHT]       = GLFW_KEY_RIGHT;
    map[SDL_SCANCODE_LEFT]        = GLFW_KEY_LEFT;
    map[SDL_SCANCODE_DOWN]        = GLFW_KEY_DOWN;
    map[SDL_SCANCODE_UP]          = GLFW_KEY_UP;

    // Keypad / Numpad
    map[SDL_SCANCODE_NUMLOCKCLEAR] = GLFW_KEY_NUM_LOCK;
    map[SDL_SCANCODE_KP_DIVIDE]    = GLFW_KEY_KP_DIVIDE;
    map[SDL_SCANCODE_KP_MULTIPLY]  = GLFW_KEY_KP_MULTIPLY;
    map[SDL_SCANCODE_KP_MINUS]     = GLFW_KEY_KP_SUBTRACT;
    map[SDL_SCANCODE_KP_PLUS]      = GLFW_KEY_KP_ADD;
    map[SDL_SCANCODE_KP_ENTER]     = GLFW_KEY_KP_ENTER;
    map[SDL_SCANCODE_KP_1]         = GLFW_KEY_KP_1;
    map[SDL_SCANCODE_KP_2]         = GLFW_KEY_KP_2;
    map[SDL_SCANCODE_KP_3]         = GLFW_KEY_KP_3;
    map[SDL_SCANCODE_KP_4]         = GLFW_KEY_KP_4;
    map[SDL_SCANCODE_KP_5]         = GLFW_KEY_KP_5;
    map[SDL_SCANCODE_KP_6]         = GLFW_KEY_KP_6;
    map[SDL_SCANCODE_KP_7]         = GLFW_KEY_KP_7;
    map[SDL_SCANCODE_KP_8]         = GLFW_KEY_KP_8;
    map[SDL_SCANCODE_KP_9]         = GLFW_KEY_KP_9;
    map[SDL_SCANCODE_KP_0]         = GLFW_KEY_KP_0;
    map[SDL_SCANCODE_KP_PERIOD]    = GLFW_KEY_KP_DECIMAL;
    map[SDL_SCANCODE_KP_EQUALS]    = GLFW_KEY_KP_EQUAL;

    // International & Application Keys
    map[SDL_SCANCODE_NONUSBACKSLASH] = GLFW_KEY_WORLD_2;
    map[SDL_SCANCODE_APPLICATION]    = GLFW_KEY_MENU;

    // Modifier Keys
    map[SDL_SCANCODE_LCTRL]  = GLFW_KEY_LEFT_CONTROL;
    map[SDL_SCANCODE_LSHIFT] = GLFW_KEY_LEFT_SHIFT;
    map[SDL_SCANCODE_LALT]   = GLFW_KEY_LEFT_ALT;
    map[SDL_SCANCODE_LGUI]   = GLFW_KEY_LEFT_SUPER;
    map[SDL_SCANCODE_RCTRL]  = GLFW_KEY_RIGHT_CONTROL;
    map[SDL_SCANCODE_RSHIFT] = GLFW_KEY_RIGHT_SHIFT;
    map[SDL_SCANCODE_RALT]   = GLFW_KEY_RIGHT_ALT;
    map[SDL_SCANCODE_RGUI]   = GLFW_KEY_RIGHT_SUPER;

    return map;
  }();

  const auto idx = std::to_underlying (scancode);
  if (std::cmp_less (idx, 0) ||
      std::cmp_greater_equal (idx, std::to_underlying (SDL_SCANCODE_COUNT)))
    return GLFW_KEY_UNKNOWN;

  return table[idx];
}