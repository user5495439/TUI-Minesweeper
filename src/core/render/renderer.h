// to do:              WriteFormatted(): implement the missing parts
// ideas to implement: RGB struct: better alpha: "transparency" and somehow also figure out a better way to make color terminal default
// cleanup:            RGB struct: remove those color static RGB structures and replace them with a cleaner alternative
//                     initBuffer(): clean up the code and arguments and maybe split it into several functions
//                     (maybe) Write functions: remove the vertical writing system because it just adds unnecessary complexity and i don't even use the system in this project and it seems kinda useless
//                     (maybe) Renderer: split Renderer into multiple files because it might be too huge for one file
//                     (maybe) Renderer: split buffer stuff into their own stuff, like make structs and/or classes related only to buffer stuff
// performance:        Renderer: cache a ton of stuff
//                     bufferWrite(): try to remove the if statements which compare RGB structs and maybe smarter memory preallocation for writeBuffer
//                     RGB struct: store the values in a single integer as hex colors

#pragma once

#include <string>
#include "colors.h"
#include "paletteColors-breeze.h"
#include "../xy.h"

namespace core::render
{
    class Renderer
    {
    public:
        enum class TextAlignment
        {
            Normal,
            Centered,
            RightToLeft
        };

        enum class TextWrap
        {
            Disabled,
            Letter,
            Word
        };

        struct RGB
        {
        public:
            unsigned char r, g, b, a; // currently aplha doesn't really work like alpha (at least how it is on other program), how it currently works is that 0 = default terminal background or foreground color

            static RGB fromInt(int value)
            {
                return RGB
                {
                    (unsigned char)((value >> 24) & 0xFF),
                    (unsigned char)((value >> 16) & 0xFF),
                    (unsigned char)((value >> 8) & 0xFF),
                    (unsigned char)(value & 0xFF)
                };
            }

            static RGB Black() { return fromInt(paletteColors[colors::Black]); }
            static RGB DarkRed() { return fromInt(paletteColors[colors::DarkRed]); }
            static RGB DarkGreen() { return fromInt(paletteColors[colors::DarkGreen]); }
            static RGB DarkYellow() { return fromInt(paletteColors[colors::DarkYellow]); }
            static RGB DarkBlue() { return fromInt(paletteColors[colors::DarkBlue]); }
            static RGB DarkMagenta() { return fromInt(paletteColors[colors::DarkMagenta]); }
            static RGB DarkCyan() { return fromInt(paletteColors[colors::DarkCyan]); }
            static RGB Gray() { return fromInt(paletteColors[colors::Gray]); }

            static RGB DarkGray() { return fromInt(paletteColors[colors::DarkGray - brightColorOffset]); }
            static RGB Red() { return fromInt(paletteColors[colors::Red - brightColorOffset]); }
            static RGB Green() { return fromInt(paletteColors[colors::Green - brightColorOffset]); }
            static RGB Yellow() { return fromInt(paletteColors[colors::Yellow - brightColorOffset]); }
            static RGB Blue() { return fromInt(paletteColors[colors::Blue - brightColorOffset]); }
            static RGB Magenta() { return fromInt(paletteColors[colors::Magenta - brightColorOffset]); }
            static RGB Cyan() { return fromInt(paletteColors[colors::Cyan - brightColorOffset]); }
            static RGB White() { return fromInt(paletteColors[colors::White - brightColorOffset]); }

            static RGB Foreground() { return fromInt(paletteColors[colors::Foreground]); }
            static RGB Background() { return fromInt(paletteColors[colors::Background]); }

            bool operator==(const RGB&) const = default;

            bool operator!=(const RGB& other) const
            {
                return !(*this == other);
            }
        private:
            static constexpr int brightColorOffset = 50;
        };

        static constexpr int leftOffset = 1;   // in linux terminals, cursor at 0 width is out of bounds
        static constexpr int topOffset = 1;    // in linux terminals, cursor at 0 height is out of bounds
        static constexpr int bottomOffset = -1;
        static constexpr int rightOffset = -1;

        static void setCursorPos(int x, int y) { setCursorPos( { x, y } ); };
        static void setCursorPos(XY xy) { cursorPos.x = xy.x - leftOffset; cursorPos.y = xy.y - topOffset; };   // the cursorPos variable stores array position
        static XY getCursorPos() { return { cursorPos.x + leftOffset, cursorPos.y + topOffset }; };
        static void Write(char character, bool vertical = false);
        static void Write(std::string string, bool vertical = false);
        static void Write(char* cString, bool vertical = false);
        static void Write(int integer, bool vertical = false);
        static size_t WriteOOB(std::string text, bool vertical = false/*, OOBBehaviour behavior*/);
        static size_t WriteFormatted(std::string text, TextAlignment alignment, TextWrap wrap = TextWrap::Disabled);
        static void setColor(RGB rgb, bool background = false) { if (background) cursorBColor = rgb; else cursorFColor = rgb; };
        static void setColor(RGB fColor, RGB bColor) { cursorBColor = bColor; cursorFColor = fColor; };
        static void bufferInitialize();
        static void bufferResize();
        static void bufferMarkAllDirty() { allDirty = true; };
        static void bufferSetClearOnWrite(bool value) { clearBufferWhenWrite = value; };
        static void bufferWrite();
        static XY getBufferDimensions() { return cellBufferDimensions; };
        static void backgroundColor(colors color) { setPaletteColor(color, true); };
        static void foregroundColor(colors color) { setPaletteColor(color, false); };
        static RGB getRGBfromPalette(colors color);
        static void setPaletteColor(colors color, bool background);
        static void setPaletteColor(colors fColor, colors bColor);
        static void resetColor() { cursorFColor = defaultFColor; cursorBColor = defaultBColor; };
        // these 3 methods below are just wrappers to the 3 methods below these 3 methods
        static char getCellChar(int x, int y) { return getCellChar( { x, y } ); };
        static RGB getCellFColor(int x, int y) { return getCellFColor( { x, y } ); };
        static RGB getCellBColor(int x, int y) { return getCellBColor( { x, y } ); };
        static char getCellChar(XY xy) { return getCell({ xy.x - leftOffset, xy.y - topOffset }).cellChar; };
        static RGB getCellFColor(XY xy) { return getCell({ xy.x - leftOffset, xy.y - topOffset }).cellFColor; };
        static RGB getCellBColor(XY xy) { return getCell({ xy.x - leftOffset, xy.y - topOffset }).cellBColor; };

    private:

        struct Cell
        {
        public:
            RGB cellFColor;
            RGB cellBColor;

            char cellChar;

            bool cellDirty;

        #ifdef DEBUG
            bool debugMark;
        #endif

            bool operator==(const Cell&) const = default;

            bool operator!=(const Cell& other) const
            {
                return !(*this == other);
            }
        };

        static const char emptyCellChar = ' ';     // what char to use for empty cells

        inline static XY cellBufferDimensions{};

        inline static XY cursorPos
        {
            0,
            0
        };

        inline static const RGB defaultFColor = RGB::Foreground();
        inline static const RGB defaultBColor = RGB::Background();

        inline static const Cell emptyCell
        {
            defaultFColor,
            defaultBColor,
            emptyCellChar,
            false
        };

        inline static RGB cursorFColor = defaultFColor;
        inline static RGB cursorBColor = defaultBColor;

        inline static bool allDirty = false;
        inline static bool clearBufferWhenWrite = false;

        inline static Cell* cellBuffer = nullptr;        // a raw dynamic array, not a vector because i want challenge
        inline static Cell* lastCellBuffer = nullptr;

        static Cell getCell(XY xy) { return cellBuffer[getBufferIndex( xy, cellBufferDimensions.x)]; }
        static int getBufferIndex(XY xy, int width) { return xy.y * width + xy.x; };
        static void initBuffer(Cell*& buffer, bool updateSizes = true, bool resize = false);
        static void findDirty(Cell* buffer, Cell* otherBuffer);
        static std::string getEscapeCursorPos(XY xy);
        static std::string getEscapeColor(RGB rgb, bool background = false);
    #ifdef DEBUG
        static bool OOBCheck(XY xy);
        static void OOBDebugMark(XY xy);
    #endif
        static size_t getDigits(int number);
        static void stdoutWrite(const char* str);
    };
}