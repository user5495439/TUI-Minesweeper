#include <cstring>
#include <string>
#include <unistd.h>
#include "renderer.h"
#include "../platform/console/console.h"
#include "colors.h"

namespace core::render
{
//public:
    void Renderer::Write(char character, bool vertical)
    {
    #ifdef DEBUG
        if (OOBCheck(cursorPos))
        {
            OOBDebugMark(cursorPos);
            if (!vertical) cursorPos.x++; else cursorPos.y++;
            return;
        }
    #endif

        Cell* cell = &cellBuffer[getBufferIndex(cursorPos, cellBufferDimensions.x)];

        if (!vertical) cursorPos.x++; else cursorPos.y++;

        if (cell->cellChar == character && cell->cellFColor == cursorFColor && cell->cellBColor == cursorBColor)
            return;

        cell->cellChar = character;

        cell->cellBColor = cursorBColor;
        cell->cellFColor = cursorFColor;

        if (!clearBufferWhenWrite) cell->cellDirty = true;
    }

    void Renderer::Write(std::string string, bool vertical)
    {
        const size_t length = string.size();    // notes for myself: length() also works but size() is more popular

        for (size_t i = 0; i < length; i++)
        {
            Write(string[i], vertical);
        }
    }

    void Renderer::Write(char* cString, bool vertical)
    {
        const size_t length = strlen(cString);

        for (size_t i = 0; i < length; i++)
        {
            Write(cString[i], vertical);
        }
    }

    void Renderer::Write(int integer, bool vertical)
    {
        Write(std::to_string(integer), vertical);
    }

    // returns how many characters of the written string are visible
    size_t Renderer::WriteOOB(std::string text, bool vertical)
    {
        XY boundsStart = {0, 0};
        XY boundsEnd = cellBufferDimensions;
        pXY cursorXY = view(cursorPos);

        // swap x and y depending on the vertical value
        if (vertical)
        {
            std::swap(boundsStart.x, boundsStart.y);
            std::swap(boundsEnd.x, boundsEnd.y);
            std::swap(cursorXY.x, cursorXY.y);
        }

        if (*cursorXY.y < boundsStart.y || *cursorXY.y >= boundsEnd.y)
            return 0;

        const size_t length = text.size();
        const int startXorY = *cursorXY.x;
        const int endXorY = startXorY + length;

        int startCut = 0;
        int endCut = 0;

        // if string start or string end is out of bounds, add values to startCut and endCut appropriately
        if (startXorY < boundsStart.x) startCut = -startXorY;
        if (endXorY > boundsEnd.x) endCut = endXorY - boundsEnd.x;

        // if the cut variables are longer than string length the string must be hidden
        const size_t cutsSum = startCut + endCut;
        if (cutsSum >= length) return 0;

        // cut the beginning and end of the string
        const size_t finalLength = length - cutsSum;
        text = text.substr(startCut, finalLength);

        // make sure cursor is in bounds before writing
        *cursorXY.x += startCut;

        Write(text, vertical);

        return finalLength;
    }

    // returns how many characters of the written string are visible
    size_t Renderer::WriteFormatted(std::string text, TextAlignment alignment, TextWrap wrap)
    {
        const size_t length = text.size();
        const XY boundsEnd = cellBufferDimensions;

        switch (alignment)
        {
            case TextAlignment::Normal:
                break;
            case TextAlignment::Centered:
                cursorPos.x -= length / 2;
                break;
            case TextAlignment::RightToLeft:
                cursorPos.x -= length;
                break;
        }

        size_t charactersWritten = 0;

        switch (wrap)
        {
            case TextWrap::Disabled:
            {
                charactersWritten = WriteOOB(text);
                break;
            }
            case TextWrap::Letter:
            {
                // makes negative values behave in a clean way
                if (cursorPos.x < 0)
                {
                    int ySubtract = cursorPos.x / boundsEnd.x - 1;
                    int x = boundsEnd.x + cursorPos.x % boundsEnd.x;

                    cursorPos = 
                    {
                        x,
                        cursorPos.y + ySubtract
                    };
                }

                size_t start = 0;

                while (start < text.size())
                {
                    size_t width = boundsEnd.x - cursorPos.x;

                    size_t count = std::min(width, text.size() - start);

                    charactersWritten += WriteOOB(text.substr(start, count));

                    start += count;

                    if (start < text.size())
                    {
                        cursorPos.x = 0;
                        cursorPos.y++;
                    }
                }
                break;
            }
            case TextWrap::Word:    // to do: implement
            {
                WriteOOB("NOT IMPLEMENTED");
                WriteOOB("NOT IMPLEMENTED");
                WriteOOB("NOT IMPLEMENTED");
                WriteOOB("NOT IMPLEMENTED");
                WriteOOB("NOT IMPLEMENTED");
                break;
            }
        }

        return charactersWritten;
    }

    void Renderer::bufferInitialize()
    {
        initBuffer(cellBuffer, true, false);

        if (clearBufferWhenWrite) initBuffer(lastCellBuffer, true, false);
    }

    void Renderer::bufferResize()
    {
        initBuffer(cellBuffer, true, true);

        if (clearBufferWhenWrite) initBuffer(lastCellBuffer, true, true);
    }

    void Renderer::bufferWrite()
    {
        constexpr char resetColorCode[] = "\x1b[0m";

        if (clearBufferWhenWrite && !allDirty)
            findDirty(cellBuffer, lastCellBuffer);

        XY sizes = cellBufferDimensions;

    #ifdef DEBUG
        for (XY xy { 0, 0 }; xy.y < sizes.y; xy.y++)
        {
            for (xy.x = 0; xy.x < sizes.x; xy.x++)
            {
                Cell* cell = &cellBuffer[getBufferIndex(xy, sizes.x)];

                if (cell->debugMark == true)
                {
                    cell->cellBColor = RGB::fromColors(DarkRed);
                    cell->cellChar = 'E';
                    cell->cellDirty = true;
                }
            }
        }
    #endif

        bool firstWrite = true;

        std::string writeBuffer = "";

        writeBuffer.reserve(sizes.x * sizes.y * 8);

        RGB previousFColor = emptyCell.cellFColor;
        RGB previousBColor = emptyCell.cellBColor;

        for (XY xy { 0, 0 }; xy.y < sizes.y; xy.y++)
        {
            int dirtyLength = 0;

            for (xy.x = 0; xy.x < sizes.x; xy.x++)
            {
                Cell* cell = &cellBuffer[getBufferIndex(xy, sizes.x)];

                if (cell->cellDirty == false && !allDirty)
                {
                    dirtyLength = 0;
                    continue;
                }

                if (cell->cellChar == '\0')
                {
                    *cell = emptyCell;
                }

                if (dirtyLength == 0)
                    appendANSICursorPos(writeBuffer, { xy.x + leftOffset, xy.y + topOffset } );

                dirtyLength++;

                if (previousFColor != cell->cellFColor || firstWrite)
                    appendANSIColor(writeBuffer, cell->cellFColor, false);

                if (previousBColor != cell->cellBColor || firstWrite)
                    appendANSIColor(writeBuffer, cell->cellBColor, true);

                previousFColor = cell->cellFColor;
                previousBColor = cell->cellBColor;

                writeBuffer += cell->cellChar;

                cell->cellDirty = false;

                firstWrite = false;
            }
        }

        writeBuffer += resetColorCode;

        stdoutWrite(writeBuffer.c_str());

        if (clearBufferWhenWrite)
        {
            // swap pointers for lastCellBuffer and cellBuffer then reinitialize cellBuffer
            std::swap(lastCellBuffer, cellBuffer);
            initBuffer(cellBuffer, false);
        }

        allDirty = false;
    }

    void Renderer::setPaletteColor(colors color, bool background)
    {
        setColor(RGB::fromColors(color), background);
    }

    void Renderer::setPaletteColor(colors fColor, colors bColor)
    {
        setColor(RGB::fromColors(fColor), RGB::fromColors(bColor));
    }

//private:
    void Renderer::initBuffer(Cell*& buffer, bool updateSizes, bool resize) // "*&" = reference or "alias"
    {
        if (!updateSizes && resize) // if the arguments are like this the method will do nothing useful so why not just return early to avoid headaches when coding this
            return;

        XY wasSizes{};

        if (updateSizes)
        {
            constexpr XY emptyXY{};

            wasSizes = cellBufferDimensions;

            XY windowSizes = platform::console::Console::windowSizes();

            cellBufferDimensions =
            {
                windowSizes.x - rightOffset - leftOffset,
                windowSizes.y - bottomOffset - topOffset
            };

            wasSizes = wasSizes == emptyXY ? cellBufferDimensions : wasSizes;
        }

        XY sizes = cellBufferDimensions;

        int bufferSize = sizes.x * sizes.y;

        if (!resize || buffer == nullptr)
        {
            if (buffer != nullptr)
                delete[] buffer;

            buffer = new Cell[bufferSize]{};

            return;
        }

        if (sizes == wasSizes)
            return;

        Cell* newCellBuffer = new Cell[bufferSize]{};

        XY maxXY = { std::min(wasSizes.x, sizes.x), std::min(wasSizes.y, sizes.y) };

        for (XY xy { 0, 0 }; xy.y < maxXY.y; xy.y++)
        {
            for (xy.x = 0; xy.x < maxXY.x; xy.x++)
            {
                newCellBuffer[getBufferIndex(xy, sizes.x)] = buffer[getBufferIndex(xy, wasSizes.x)];
            }
        }

        delete[] buffer;

        buffer = newCellBuffer;
    }

    // both buffers must be as wide as cellBufferWidth and as tall as cellBufferHeight
    void Renderer::findDirty(Cell* buffer, Cell* otherBuffer)
    {
        XY sizes = cellBufferDimensions;

        for (XY xy { 0, 0 }; xy.y < sizes.y; xy.y++)
        {
            for (xy.x = 0; xy.x < sizes.x; xy.x++)
            {
                Cell* cell = &buffer[getBufferIndex(xy, sizes.x)];
                Cell* otherCell = &otherBuffer[getBufferIndex(xy, sizes.x)];

                if (*cell == *otherCell)
                {
                    cell->cellDirty = false;
                }

                else
                {
                    cell->cellDirty = true;
                }
            }
        }
    }

    void Renderer::appendANSICursorPos(std::string& buffer, XY xy)
    {
        buffer += "\x1b[";
        buffer += std::to_string(xy.y);
        buffer += ";";
        buffer += std::to_string(xy.x);
        buffer += "H";
    }

    // apparently you can combine both foreground and background ANSI escape sequences into a single escape sequence, so that's an optimization waiting to be implemented
    void Renderer::appendANSIColor(std::string& buffer, RGB rgb, bool background)
    {
        if (*rgb.a == 0)
        {
            appendANSIDefaultColor(buffer, background);
            return;
        }

        buffer += "\x1b[";
        buffer += std::to_string(38 + background * 10);
        buffer += ";2;";
        buffer += std::to_string(*rgb.r);
        buffer += ";";
        buffer += std::to_string(*rgb.g);
        buffer += ";";
        buffer += std::to_string(*rgb.b);
        buffer += "m";
    }

    void Renderer::appendANSIDefaultColor(std::string& buffer, bool background)
    {
        buffer += "\x1b[";
        buffer += std::to_string(39 + background * 10);
        buffer += "m";
    }

#ifdef DEBUG
    bool Renderer::OOBCheck(XY xy)
    {
        if (xy.x < 0 || xy.y < 0 || xy.x > cellBufferDimensions.x - 1 || xy.y > cellBufferDimensions.y - 1)
            return true;

        return false;
    }

    void Renderer::OOBDebugMark(XY xy)
    {
        xy.x = std::max(xy.x, 0);
        xy.y = std::max(xy.y, 0);

        xy.x = std::min(xy.x, cellBufferDimensions.x - 1);
        xy.y = std::min(xy.y, cellBufferDimensions.y - 1);

        cellBuffer[getBufferIndex(xy, cellBufferDimensions.x)].debugMark = true;
    }
#endif

    size_t Renderer::getDigits(int number)
    {
        size_t digits = 1;

        while (number >= 10)
        {
            number /= 10;
            digits++;
        }

        return digits;
    }

    void Renderer::stdoutWrite(const char* str)
    {
        write(STDOUT_FILENO, str, strlen(str));
    }
}