#pragma once
#include "CommonHeader.h"
#include <cmath>
#include <iostream>

using std::sqrt;

namespace gfx
{
    class vec3f
    {
    public:
        vec3f() : x(0.f), y(0.f), z(0.f) {}
        vec3f(const f32 value) : x(value), y(value), z(value) {}
        vec3f(const f32 x, const f32 y, const f32 z) : x(x), y(y), z(z) {}
        vec3f(const u32 x, const u32 y, const u32 z) : x(static_cast<f32>(x)), y(static_cast<f32>(y)), z(static_cast<f32>(z)) {}

    public:
        vec3f operator-() const { return vec3f(-x, -y, -z); }

        vec3f& operator+=(const vec3f& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        vec3f& operator*=(const f32 t)
        {
            x *= t;
            y *= t;
            z *= t;
            return *this;
        }

        vec3f& operator/=(const f32 t)
        {
            x /= t;
            y /= t;
            z /= t;
            return *this;
        }

    public:
        const f32 Dot(const vec3f& other) const
        {
            return x * other.x
                + y * other.y
                + z * other.z;
        }

        const bool IsNearlyZero() const
        {
            return Dot(*this) < 1e-5 * 1e-5;
        }

        const f32 Length() const
        {
            return sqrt(LengthSqr());
        }

        const f32 LengthSqr() const
        {
            return x * x + y * y + z * z;
        }

        static const vec3f Random()
        {
            return vec3f(Random::RandomFloat(), Random::RandomFloat(), Random::RandomFloat());
        }

        static const vec3f Random(f32 min, f32 max)
        {
            return vec3f(Random::RandomFloat(min, max), Random::RandomFloat(min, max), Random::RandomFloat(min, max));
        }

        static const vec3f RandomInUnitSphere()
        {
            while (true)
            {
                vec3f p = vec3f::Random(-1.f, 1.f);
                if (p.LengthSqr() >= 1.f) continue;
                return p;
            }
        }

        static const vec3f RandomInHemisphere(const vec3f& normal)
        {
            vec3f result = RandomInUnitSphere();
            if (result.Dot(normal) < 0.f) result *= -1.f; // turns sphere into hemisphere
            return result;
        }

        static const vec3f RandomInUnitDisk()
        {
            while (true)
            {
                vec3f p = vec3f(Random::RandomFloat(-1.f, 1.f), Random::RandomFloat(-1.f, 1.f), 0.f);
                if (p.LengthSqr() >= 1.f) continue;
                return p;
            }
        }

    public:
        f32 x;
        f32 y;
        f32 z;
    };

    // vec3f Utility Functions

    inline std::ostream& operator<<(std::ostream& out, const vec3f& v)
    {
        return out << v.x << ' ' << v.y << ' ' << v.z;
    }

    inline vec3f operator+(const vec3f& u, const vec3f& v)
    {
        return vec3f(u.x + v.x, u.y + v.y, u.z + v.z);
    }

    inline vec3f operator-(const vec3f& u, const vec3f& v)
    {
        return vec3f(u.x - v.x, u.y - v.y, u.z - v.z);
    }

    inline vec3f operator*(const vec3f& u, const vec3f& v)
    {
        return vec3f(u.x * v.x, u.y * v.y, u.z * v.z);
    }

    inline vec3f operator*(f32 t, const vec3f& v)
    {
        return vec3f(t * v.x, t * v.y, t * v.z);
    }

    inline vec3f operator*(const vec3f& v, f32 t)
    {
        return t * v;
    }

    inline vec3f operator/(vec3f v, f32 t)
    {
        return (1.f / t) * v;
    }

    inline vec3f operator/(vec3f u, vec3f v)
    {
        return vec3f(u.x / v.x, u.y / v.y, u.z / v.z);
    }

    inline f32 Dot(const vec3f& u, const vec3f& v)
    {
        return u.x * v.x
            + u.y * v.y
            + u.z * v.z;
    }

    inline vec3f Cross(const vec3f& u, const vec3f& v)
    {
        return vec3f(u.y * v.z - u.z * v.y,
            u.z * v.x - u.x * v.z,
            u.x * v.y - u.y * v.x);
    }

    inline vec3f Lerp(const vec3f& u, const vec3f& v, const f32 lerp)
    {
        const f32 rcpLerp = 1.f - lerp;
        return vec3f(u.x * rcpLerp + lerp * v.x,
            u.y * rcpLerp + lerp * v.y,
            u.z * rcpLerp + lerp * v.z);
    }

    inline vec3f Reflect(const vec3f& v, const vec3f& n)
    {
        return v - 2.f * Dot(v, n) * n;
    }

    inline vec3f Refract(const vec3f& uv, const vec3f& n, f32 etai_over_etat)
    {
        const f32 cosTheta = fmin(Dot(-uv, n), 1.f);
        const vec3f r_out_perp = etai_over_etat * (uv + cosTheta * n);
        const vec3f r_out_parallel = -sqrt(fabs(1.f - r_out_perp.LengthSqr())) * n;
        return r_out_perp + r_out_parallel;
    }

    inline vec3f Normalize(const vec3f& v)
    {
        return v / v.Length();
    }

    inline vec3f MinVec3(const vec3f& a, const vec3f& b)
    {
        return vec3f(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
    }

    inline vec3f MaxVec3(const vec3f& a, const vec3f& b)
    {
        return vec3f(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
    }

    inline vec3f SCurve(const vec3f& v)
    {
        return v * v * (3.f - 2.f * v);
    }
}