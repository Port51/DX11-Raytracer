#pragma once
#include "CommonHeader.h"
#include <cmath>
#include <iostream>

using std::sqrt;

namespace gfx
{
    class ivec3
    {
    public:
        ivec3() : x(0u), y(0u), z(0u) {}
        ivec3(const int32_t value) : x(value), y(value), z(value) {}
        ivec3(const int32_t x, const int32_t y, const int32_t z) : x(x), y(y), z(z) {}

    public:
        ivec3& operator+=(const ivec3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        ivec3& operator*=(const int32_t t)
        {
            x *= t;
            y *= t;
            z *= t;
            return *this;
        }

        ivec3& operator/=(const int32_t t)
        {
            return *this *= 1 / t;
        }

    public:
        const int32_t Dot(const ivec3& other) const
        {
            return x * other.x
                + y * other.y
                + z * other.z;
        }

    public:
        int32_t x;
        int32_t y;
        int32_t z;
    };

    // ivec3 Utility Functions

    inline ivec3 operator<< (ivec3 v, std::size_t n)
    {
        return ivec3(v.x << n, v.y << n, v.z << n);
    }

    inline ivec3 operator>> (ivec3 v, std::size_t n)
    {
        return ivec3(v.x >> n, v.y >> n, v.z >> n);
    }

    inline ivec3 operator^ (ivec3 u, ivec3 v)
    {
        return ivec3(u.x ^ v.x, u.y ^ v.y, u.z ^ v.z);
    }

    inline ivec3 operator% (ivec3 u, ivec3 v)
    {
        return ivec3(u.x % v.x, u.y % v.y, u.z % v.z);
    }

    inline std::ostream& operator<<(std::ostream& out, const ivec3& v)
    {
        return out << v.x << ' ' << v.y << ' ' << v.z;
    }

    inline ivec3 operator+(const ivec3& u, const ivec3& v)
    {
        return ivec3(u.x + v.x, u.y + v.y, u.z + v.z);
    }

    inline ivec3 operator-(const ivec3& u, const ivec3& v)
    {
        return ivec3(u.x - v.x, u.y - v.y, u.z - v.z);
    }

    inline ivec3 operator*(const ivec3& u, const ivec3& v)
    {
        return ivec3(u.x * v.x, u.y * v.y, u.z * v.z);
    }

    inline ivec3 operator*(int32_t t, const ivec3& v)
    {
        return ivec3(t * v.x, t * v.y, t * v.z);
    }

    inline ivec3 operator*(const ivec3& v, int32_t t)
    {
        return t * v;
    }

    inline ivec3 operator/(ivec3 v, int32_t t)
    {
        return ivec3(v.x / t, v.y / t, v.z / t);
    }

    inline ivec3 Cross(const ivec3& u, const ivec3& v)
    {
        return ivec3(u.y * v.z - u.z * v.y,
            u.z * v.x - u.x * v.z,
            u.x * v.y - u.y * v.x);
    }
}