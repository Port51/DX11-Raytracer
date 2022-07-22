#pragma once
#include "CommonHeader.h"
#include <cmath>
#include <iostream>

using std::sqrt;

namespace gfx
{
    class vec3
    {
    public:
        vec3() : x(0), y(0), z(0) {}
        vec3(const double value) : x(value), y(value), z(value) {}
        vec3(const double x, const double y, const double z) : x(x), y(y), z(z) {}

    public:
        vec3 operator-() const { return vec3(-x, -y, -z); }

        vec3& operator+=(const vec3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        vec3& operator*=(const double t)
        {
            x *= t;
            y *= t;
            z *= t;
            return *this;
        }

        vec3& operator/=(const double t)
        {
            return *this *= 1 / t;
        }

    public:
        const double Dot(const vec3& other) const
        {
            return x * other.x
                + y * other.y
                + z * other.z;
        }

        const bool IsNearlyZero() const
        {
            return Dot(*this) < 1e-5 * 1e-5;
        }

        const double Length() const
        {
            return sqrt(LengthSqr());
        }

        const double LengthSqr() const
        {
            return x * x + y * y + z * z;
        }

        static const vec3 Random()
        {
            return vec3(Random::RandomDouble(), Random::RandomDouble(), Random::RandomDouble());
        }

        static const vec3 Random(double min, double max)
        {
            return vec3(Random::RandomDouble(min, max), Random::RandomDouble(min, max), Random::RandomDouble(min, max));
        }

        static const vec3 RandomInUnitSphere()
        {
            while (true)
            {
                auto p = vec3::Random(-1.0, 1.0);
                if (p.LengthSqr() >= 1.0) continue;
                return p;
            }
        }

        static const vec3 RandomInHemisphere(const vec3& normal)
        {
            vec3 result = RandomInUnitSphere();
            if (result.Dot(normal) < 0.0) result *= -1.0; // turns sphere into hemisphere
            return result;
        }

        static const vec3 RandomInUnitDisk()
        {
            while (true)
            {
                auto p = vec3(Random::RandomDouble(-1.0, 1.0), Random::RandomDouble(-1.0, 1.0), 0.0);
                if (p.LengthSqr() >= 1.0) continue;
                return p;
            }
        }

    public:
        double x;
        double y;
        double z;
    };

    // vec3 Utility Functions

    inline std::ostream& operator<<(std::ostream& out, const vec3& v)
    {
        return out << v.x << ' ' << v.y << ' ' << v.z;
    }

    inline vec3 operator+(const vec3& u, const vec3& v)
    {
        return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
    }

    inline vec3 operator-(const vec3& u, const vec3& v)
    {
        return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
    }

    inline vec3 operator*(const vec3& u, const vec3& v)
    {
        return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
    }

    inline vec3 operator*(double t, const vec3& v)
    {
        return vec3(t * v.x, t * v.y, t * v.z);
    }

    inline vec3 operator*(const vec3& v, double t)
    {
        return t * v;
    }

    inline vec3 operator/(vec3 v, double t)
    {
        return (1 / t) * v;
    }

    inline vec3 operator/(vec3 u, vec3 v)
    {
        return vec3(u.x / v.x, u.y / v.y, u.z / v.z);
    }

    inline double Dot(const vec3& u, const vec3& v)
    {
        return u.x * v.x
            + u.y * v.y
            + u.z * v.z;
    }

    inline vec3 Cross(const vec3& u, const vec3& v)
    {
        return vec3(u.y * v.z - u.z * v.y,
            u.z * v.x - u.x * v.z,
            u.x * v.y - u.y * v.x);
    }

    inline vec3 Reflect(const vec3& v, const vec3& n)
    {
        return v - 2.0 * Dot(v, n) * n;
    }

    inline vec3 Refract(const vec3& uv, const vec3& n, double etai_over_etat)
    {
        auto cos_theta = fmin(Dot(-uv, n), 1.0);
        vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
        vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.LengthSqr())) * n;
        return r_out_perp + r_out_parallel;
    }

    inline vec3 Normalize(const vec3& v)
    {
        return v / v.Length();
    }

    inline vec3 MinVec3(const vec3& a, const vec3& b)
    {
        return vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
    }

    inline vec3 MaxVec3(const vec3& a, const vec3& b)
    {
        return vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
    }

    inline vec3 SCurve(const vec3& v)
    {
        return v * v * (3.0 - 2.0 * v);
    }
}