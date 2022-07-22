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

        const bool IsNearlyZero() const
        {
            return Dot(*this) < 1e-5 * 1e-5;
        }

        const int32_t Length() const
        {
            return sqrt(LengthSqr());
        }

        const int32_t LengthSqr() const
        {
            return x * x + y * y + z * z;
        }

        static const ivec3 Random()
        {
            return ivec3(Random::RandomDouble(), Random::RandomDouble(), Random::RandomDouble());
        }

        static const ivec3 Random(int32_t min, int32_t max)
        {
            return ivec3(Random::RandomDouble(min, max), Random::RandomDouble(min, max), Random::RandomDouble(min, max));
        }

        static const ivec3 RandomInUnitSphere()
        {
            while (true)
            {
                auto p = ivec3::Random(-1.0, 1.0);
                if (p.LengthSqr() >= 1.0) continue;
                return p;
            }
        }

        static const ivec3 RandomInHemisphere(const ivec3& normal)
        {
            ivec3 result = RandomInUnitSphere();
            if (result.Dot(normal) < 0.0) result *= -1.0; // turns sphere into hemisphere
            return result;
        }

        static const ivec3 RandomInUnitDisk()
        {
            while (true)
            {
                auto p = ivec3(Random::RandomDouble(-1.0, 1.0), Random::RandomDouble(-1.0, 1.0), 0.0);
                if (p.LengthSqr() >= 1.0) continue;
                return p;
            }
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
        return (1 / t) * v;
    }

    inline ivec3 Cross(const ivec3& u, const ivec3& v)
    {
        return ivec3(u.y * v.z - u.z * v.y,
            u.z * v.x - u.x * v.z,
            u.x * v.y - u.y * v.x);
    }
}