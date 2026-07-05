#include "xy.h"

namespace core
{
    //struct XY
    //{
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

        XY& XY::operator+=(int value)
        {
            x += value;
            y += value;
            return *this;
        }

        XY& XY::operator-=(int value)
        {
            x -= value;
            y -= value;
            return *this;
        }

        bool XY::operator!=(const XY& other) const
        {
            return !(*this == other);
        }

        XY& XY::operator=(int value)
        {
            x = value;
            y = value;
            return *this;
        }
    //}

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

    XY operator+(XY lhs, const int rhs)
    {
        lhs += rhs;
        return lhs;
    }

    XY operator-(XY lhs, const int rhs)
    {
        lhs += rhs;
        return lhs;
    }

    XY value(const pXY& p)
    {
        return { *p.x, *p.y };
    }


    //struct pXY
    //{
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

        pXY& pXY::operator+=(int value)
        {
            *x += value;
            *y += value;
            return *this;
        }

        pXY& pXY::operator-=(int value)
        {
            *x -= value;
            *y -= value;
            return *this;
        }

        bool pXY::operator!=(const pXY& other) const
        {
            return !(*this == other);
        }

        pXY& pXY::operator=(int value)
        {
            *x = value;
            *y = value;
            return *this;
        }
    //}

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

    pXY operator+(pXY lhs, const int rhs)
    {
        lhs += rhs;
        return lhs;
    }

    pXY operator-(pXY lhs, const int rhs)
    {
        lhs += rhs;
        return lhs;
    }

    pXY view(XY& xy)
    {
        return { &xy.x, &xy.y };
    }
}