// to do:              WriteFormatted(): implement the missing parts
// ideas to implement: RGB struct: better alpha: "transparency" and somehow also figure out a better way to make color terminal default
// cleanup:            initBuffer(): clean up the code and arguments and maybe split it into several functions
//                     (maybe) Write functions: remove the vertical writing system because it just adds unnecessary complexity and i don't even use the system in this project and it seems kinda useless
//                     (maybe) Renderer: split Renderer into multiple files because it might be too huge for one file
//                     (maybe) Renderer: split buffer stuff into their own stuff, like make structs and/or classes related only to buffer stuff
// performance:        Renderer: cache a ton of stuff
//                     bufferWrite(): try to remove the if statements which compare RGB structs and maybe smarter memory preallocation for writeBuffer

#pragma once

#include <string>
#include "rgb.h"
#include "colors.h"
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

            bool operator==(const Cell& other) const
            {
                return cellFColor == other.cellFColor &&
                    cellBColor == other.cellBColor &&
                    cellChar == other.cellChar;
            }

            bool operator!=(const Cell& other) const
            {
                return !(*this == other);
            }
        };

        static const char emptyCellChar = ' ';     // what char to use for empty cells

        static inline XY cellBufferDimensions{};

        static inline XY cursorPos
        {
            0,
            0
        };

        static inline const RGB defaultFColor = RGB::fromColors(Foreground);
        static inline const RGB defaultBColor = RGB::fromColors(Background);

        static inline const Cell emptyCell
        {
            defaultFColor,
            defaultBColor,
            emptyCellChar,
            false
        };

        static inline RGB cursorFColor = defaultFColor;
        static inline RGB cursorBColor = defaultBColor;

        static inline bool allDirty = false;
        static inline bool clearBufferWhenWrite = false;

        static inline Cell* cellBuffer = nullptr;        // a raw dynamic array, not a vector because i want challenge
        static inline Cell* lastCellBuffer = nullptr;

        static Cell getCell(XY xy) { return cellBuffer[getBufferIndex( xy, cellBufferDimensions.x)]; }
        static int getBufferIndex(XY xy, int width) { return xy.y * width + xy.x; };
        static void initBuffer(Cell*& buffer, bool updateSizes = true, bool resize = false);
        static void findDirty(Cell* buffer, Cell* otherBuffer);
        static void appendANSICursorPos(std::string& buffer, XY xy);
        static void appendANSIColor(std::string& buffer, RGB rgb, bool background = false);
        static void appendANSIDefaultColor(std::string& buffer, bool background);
    #ifdef DEBUG
        static bool OOBCheck(XY xy);
        static void OOBDebugMark(XY xy);
    #endif
        static size_t getDigits(int number);
        static void stdoutWrite(const char* str);
    };
}