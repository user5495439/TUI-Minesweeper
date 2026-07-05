#pragma once

#include "../../core/render/renderer.h"
#include <string>

namespace game::render
{
    using namespace core::render;

    class GameRender
    {
    public:
        struct Sides
        {
            int top;
            int bottom;
            int left;
            int right;
        };

        static void drawTiles(core::XY offsets);
        static void drawUI(core::XY offsets);
    #ifdef DEBUG
        static void drawDebug();
    #endif

    private:
        struct TileAppearance
        {
            std::string icon;
            colors fColor;
            colors bColor;
        };

        inline static const TileAppearance hiddenTileAppearance[]
        {
            {"[]", Gray, DarkGray},
            {"  ", Black, Black},
            {"[]", Red, DarkGray},
            {"??", Black, DarkGray},
            {"[]", DarkGray, Red},
            {"<>", DarkGray, DarkRed},
            {"  ", Background, White},
            {"  ", Foreground, Background}
        };

        inline static const TileAppearance revealedTileAppearance[]
        {
            { "  ", Black, Black },
            { "01", Blue, Black },
            { "02", Green, Black },
            { "03", Red, Black },
            { "04", Magenta, Black },
            { "05", DarkYellow, Black },
            { "06", Cyan, Black },
            { "07", DarkGray, Black },
            { "08", DarkRed, Black },
            { "<>", DarkGray, Black },
            { "??", Foreground, Red }
        };

        inline static const TileAppearance revealedTileAppearanceAlt[]
        {
            { "  ", Black, Black },
            { "11", Blue, Black },
            { "22", Green, Black },
            { "33", Red, Black },
            { "44", Magenta, Black },
            { "55", DarkYellow, Black },
            { "66", Cyan, Black },
            { "77", DarkGray, Black },
            { "88", DarkRed, Black },
            { "<>", DarkGray, Black },
            { "??", Foreground, Red }
        };

        static Sides getCuts(core::XY sizes, core::XY offsets);
        static std::string formatMineswNumber(int input, size_t max);
    };
}