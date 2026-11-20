#pragma once

#include "scancodes_linux.h"

struct sdl_scancode_table_from_linux {
    Uint16 map[SDL_SCANCODE_COUNT];
};

constexpr int linux_scancode_table_size =
  (int) (sizeof (linux_scancode_table) / sizeof (linux_scancode_table[0]));

constexpr sdl_scancode_table_from_linux
build_sdl_scancode_table_from_linux ()
{
  sdl_scancode_table_from_linux t{};

  for (int i = 0; i < linux_scancode_table_size; ++i)
    {
      const SDL_Scancode sc = linux_scancode_table[i];
      if (sc == SDL_SCANCODE_UNKNOWN || (int) sc < 0 || (int) sc >= SDL_SCANCODE_COUNT)
        continue;

      if (t.map[sc] == 0)
        t.map[sc] = (Uint16)i;
    }

  // Some overrides. Fun research :)
  t.map[SDL_SCANCODE_MEDIA_PLAY]  = 207;  // KEY_PLAY over KEY_PLAYCD
  t.map[SDL_SCANCODE_AGAIN]    = 182;  // KEY_REDO instead of KEY_AGAIN

  return t;
}

// I am now going to promptly forget that I ever wrote this.

static constexpr
sdl_scancode_table_from_linux sdl_to_linux_table =
  build_sdl_scancode_table_from_linux();

constexpr int
sdl_scancode_to_linux (SDL_Scancode sc)
{
    return ((int) sc >= 0 && (int) sc < SDL_SCANCODE_COUNT) ? sdl_to_linux_table.map[sc] : 0;
}

constexpr SDL_Scancode
linux_to_sdl_scancode (int linux_code)
{
  return (linux_code >= 0 && linux_code < linux_scancode_table_size)
           ? linux_scancode_table[linux_code]
           : SDL_SCANCODE_UNKNOWN;
}
