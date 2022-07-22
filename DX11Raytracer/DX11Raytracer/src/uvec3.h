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

        const bool IsNearlyZero() const
        {
            return Dot(*this) < 1e-5 * 1e-5;
        }

        const uint32_t Length() const
        {
            return sqrt(LengthSqr());
        }

        const uint32_t LengthSqr() const
        {
            return x * x + y * y + z * z;
        }

        static const uvec3 Random()
        {
            return uvec3(Random::RandomDouble(), Random::RandomDouble(), Random::RandomDouble());
        }

        static const uvec3 Random(uint32_t min, uint32_t max)
        {
            return uvec3(Random::RandomDouble(min, max), Random::RandomDouble(min, max), Random::RandomDouble(min, max));
        }

        static const uvec3 RandomInUnitSphere()
        {
            while (true)
            {
                auto p = uvec3::Random(-1.0, 1.0);
                if (p.LengthSqr() >= 1.0) continue;
                return p;
            }
        }

        static const uvec3 RandomInHemisphere(const uvec3& normal)
        {
            uvec3 result = RandomInUnitSphere();
            if (result.Dot(normal) < 0.0) result *= -1.0; // turns sphere into hemisphere
            return result;
        }

        static const uvec3 RandomInUnitDisk()
        {
            while (true)
            {
                auto p = uvec3(Random::RandomDouble(-1.0, 1.0), Random::RandomDouble(-1.0, 1.0), 0.0);
                if (p.LengthSqr() >= 1.0) continue;
                return p;
            }
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
        return (1 / t) * v;
    }

    inline uvec3 Cross(const uvec3& u, const uvec3& v)
    {
        return uvec3(u.y * v.z - u.z * v.y,
            u.z * v.x - u.x * v.z,
            u.x * v.y - u.y * v.x);
    }
}