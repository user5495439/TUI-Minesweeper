#pragma once

#include <cstdint>
#include "colors.h"
#include "paletteColors-breeze.h"

namespace core::render
{
    struct RGB
    {
    public:
        uint32_t value;

        uint8_t* const valueBytePtr = reinterpret_cast<uint8_t*>(&value);

        // constant pointers, so pointer adress can't be modified but the value the pointer points to can be modified
        uint8_t* const r = valueBytePtr + 3;
        uint8_t* const g = valueBytePtr + 2;
        uint8_t* const b = valueBytePtr + 1;
        uint8_t* const a = valueBytePtr;

        static RGB fromColors(colors color)
        {
            return RGB
            {
                paletteColors[color]
            };
        }

        RGB& operator=(const RGB& other)
        {
            value = other.value;
            return *this;
        }

        bool operator==(const RGB& other) const
        {
            return value == other.value;
        }

        bool operator!=(const RGB& other) const
        {
            return !(*this == other);
        }

    private:
    };
}