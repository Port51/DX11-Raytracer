#pragma once
#include "CommonHeader.h"
#include <cmath>
#include <iostream>

using std::sqrt;

namespace gfx
{
    class uvec3
    {
    public:
        uvec3() : x(0u), y(0u), z(0u) {}
        uvec3(const uint32_t value) : x(value), y(value), z(value) {}
        uvec3(const uint32_t x, const uint32_t y, const uint32_t z) : x(x), y(y), z(z) {}
        uvec3(const float x, const float y, const float z) : x(static_cast<uint32_t>(x)), y(static_cast<uint32_t>(y)), z(static_cast<uint32_t>(z)) {}
        uvec3(const double x, const double y, const double z) : x(static_cast<uint32_t>(x)), y(static_cast<uint32_t>(y)), z(static_cast<uint32_t>(z)) {}

    public:
        uvec3& operator+=(const uvec3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        uvec3& operator*=(const uint32_t t)
        {
            x *= t;
            y *= t;
            z *= t;
            return *this;
        }

        uvec3& operator/=(const uint32_t t)
        {
            return *this *= 1 / t;
        }

    public:
        const uint32_t Dot(const uvec3& other) const
        {
            return x * other.x
                + y * other.y
                + z * other.z;
        }

    public:
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };

    // uvec3 Utility Functions

    inline uvec3 operator<< (uvec3 v, std::size_t n)
    {
        return uvec3(v.x << n, v.y << n, v.z << n);
    }

    inline uvec3 operator>> (uvec3 v, std::size_t n)
    {
        return uvec3(v.x >> n, v.y >> n, v.z >> n);
    }

    inline uvec3 operator^ (uvec3 u, uvec3 v)
    {
        return uvec3(u.x ^ v.x, u.y ^ v.y, u.z ^ v.z);
    }

    inline uvec3 operator% (uvec3 u, uvec3 v)
    {
        return uvec3(u.x % v.x, u.y % v.y, u.z % v.z);
    }

    inline std::ostream& operator<<(std::ostream& out, const uvec3& v)
    {
        return out << v.x << ' ' << v.y << ' ' << v.z;
    }

    inline uvec3 operator+(const uvec3& u, const uvec3& v)
    {
        return uvec3(u.x + v.x, u.y + v.y, u.z + v.z);
    }

    inline uvec3 operator-(const uvec3& u, const uvec3& v)
    {
        return uvec3(u.x - v.x, u.y - v.y, u.z - v.z);
    }

    inline uvec3 operator*(const uvec3& u, const uvec3& v)
    {
        return uvec3(u.x * v.x, u.y * v.y, u.z * v.z);
    }

    inline uvec3 operator*(uint32_t t, const uvec3& v)
    {
        return uvec3(t * v.x, t * v.y, t * v.z);
    }

    inline uvec3 operator*(const uvec3& v, uint32_t t)
    {
        return t * v;
    }

    inline uvec3 operator/(uvec3 v, uint32_t t)
    {
        return uvec3(v.x / t, v.y / t, v.z / t);
    }

    inline uvec3 Cross(const uvec3& u, const uvec3& v)
    {
        return uvec3(u.y * v.z - u.z * v.y,
            u.z * v.x - u.x * v.z,
            u.x * v.y - u.y * v.x);
    }
}