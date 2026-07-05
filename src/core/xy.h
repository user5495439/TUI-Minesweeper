#pragma once

namespace core
{
    struct pXY;

    struct XY
    {
    public:
        int x;
        int y;

        XY& operator+=(const XY& other);
        XY& operator-=(const XY& other);

        XY& operator+=(int value);
        XY& operator-=(int value);

        bool operator==(const XY&) const = default;
        bool operator!=(const XY& other) const;

        XY& operator=(int value);
    };

    XY operator+(XY lhs, const XY& rhs);
    XY operator-(XY lhs, const XY& rhs);

    XY operator+(XY lhs, const int rhs);
    XY operator-(XY lhs, const int rhs);

    XY value(const pXY& p);


    struct pXY
    {
    public:
        int* x = nullptr;
        int* y = nullptr;

        pXY& operator+=(const pXY& other);
        pXY& operator-=(const pXY& other);

        pXY& operator+=(int value);
        pXY& operator-=(int value);

        bool operator==(const pXY&) const = default;
        bool operator!=(const pXY& other) const;

        pXY& operator=(int value);
    };

    pXY operator+(pXY lhs, const pXY& rhs);
    pXY operator-(pXY lhs, const pXY& rhs);

    pXY operator+(pXY lhs, const int rhs);
    pXY operator-(pXY lhs, const int rhs);

    pXY view(XY& xy);
}