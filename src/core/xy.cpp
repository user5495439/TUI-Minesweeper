#include "xy.h"

namespace core
{
    XY& XY::operator+=(const XY& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    XY& XY::operator-=(const XY& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    bool XY::operator!=(const XY& other) const
    {
        return !(*this == other);
    }

    XY operator+(XY lhs, const XY& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    XY operator-(XY lhs, const XY& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    XY value(const pXY& p)
    {
        return { *p.x, *p.y };
    }


    pXY& pXY::operator+=(const pXY& other)
    {
        *x += *other.x;
        *y += *other.y;
        return *this;
    }

    pXY& pXY::operator-=(const pXY& other)
    {
        *x -= *other.x;
        *y -= *other.y;
        return *this;
    }

    bool pXY::operator!=(const pXY& other) const
    {
        return !(*this == other);
    }

    pXY operator+(pXY lhs, const pXY& rhs)
    {
        lhs += rhs;
        return lhs;
    }

    pXY operator-(pXY lhs, const pXY& rhs)
    {
        lhs -= rhs;
        return lhs;
    }

    pXY view(XY& xy)
    {
        return { &xy.x, &xy.y };
    }
}