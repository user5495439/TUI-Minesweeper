#pragma once

#include <array>
#include <cstdint>

namespace core::render
{
    inline const static std::array<uint32_t, 30> paletteColors =
    {
        0x232627ff,   // 0, BLACK
        0xed1515ff,   // 1, RED
        0x11d116ff,   // 2, GREEN
        0xf67400ff,   // 3, YELLOW
        0x1d99f3ff,   // 4, BLUE
        0x9b59b6ff,   // 5, PURPLE
        0x1abc9cff,   // 6, CYAN
        0xfcfcfcff,   // 7, WHITE
        0xfcfcfc00,   // Foreground default (8)
        0x23262700,   // Background default (9)

        0x7f8c8dff,   // 10, BRIGHT_BLACK
        0xc0392bff,   // 11, BRIGHT_RED
        0x1cdc9aff,   // 12, BRIGHT_GREEN
        0xfdbc4bff,   // 13, BRIGHT_YELLOW
        0x3daee9ff,   // 14, BRIGHT_BLUE
        0x8e44adff,   // 15, BRIGHT_PURPLE
        0x16a085ff,   // 16, BRIGHT_CYAN
        0xffffffff,   // 17, BRIGHT_WHITE
        0x3daee900,   // BRIGHT_Foreground default (18)
        0x00000000,   // BRIGHT_Background default (19)

        0x31363bff,   // 20, FAINT_BLACK
        0x783228ff,   // 21, FAINT_RED
        0x17a262ff,   // 22, FAINT_GREEN
        0xb65619ff,   // 23, FAINT_YELLOW
        0x1b668fff,   // 24, FAINT_BLUE
        0x614a73ff,   // 25, FAINT_PURPLE
        0x186c60ff,   // 26, FAINT_CYAN
        0x63686dff,   // 27, FAINT_WHITE
        0xeff0f100,   // FAINT_Foreground default (28)
        0x31363b00    // FAINT_Background default (29)
    };
}