#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class PerlinNoise
    {
    public:
        PerlinNoise() = default;
        ~PerlinNoise() = default;

        static const double GetNoise3D(vec3 p, const uint octaveCt)
        {
            p += vec3(1000000.0, 1000000.0, 1000000.0);
            const uvec3 u = uvec3(p.x, p.y, p.z);

            auto v = 0.0;
            auto scale = 1.0;
            uint pitch = 64u;
            for (int i = 0; i < octaveCt; ++i)
            {
                const uvec3 u2 = u - (u % uvec3(pitch));
                const vec3 lerp = SCurve((p - vec3(u2.x, u2.y, u2.z)) / pitch);

                const auto s000 = HashToNoise(Hash(u2));
                const auto s100 = HashToNoise(Hash(u2 + uvec3(pitch, 0u, 0u)));
                const auto s010 = HashToNoise(Hash(u2 + uvec3(0u, pitch, 0u)));
                const auto s110 = HashToNoise(Hash(u2 + uvec3(pitch, pitch, 0u)));
                const auto s001 = HashToNoise(Hash(u2 + uvec3(0u, 0u, pitch)));
                const auto s101 = HashToNoise(Hash(u2 + uvec3(pitch, 0u, pitch)));
                const auto s011 = HashToNoise(Hash(u2 + uvec3(0u, pitch, pitch)));
                const auto s111 = HashToNoise(Hash(u2 + uvec3(pitch, pitch, pitch)));

                v += scale * Lerp(
                    Lerp(Lerp(s000, s100, lerp.x), Lerp(s010, s110, lerp.x), lerp.y),
                    Lerp(Lerp(s001, s101, lerp.x), Lerp(s011, s111, lerp.x), lerp.y), lerp.z);
                auto vv = Lerp(s000, s100, lerp.x);
                scale *= 0.5;
                pitch /= 2u;
            }

            return v;
        }

    private:
        // Hash without Sine
        // https://www.shadertoy.com/view/4djSRW
        static const float hashwithoutsine11(float p)
        {
            p = Frac(p * 0.1031);
            p *= p + 33.33;
            p *= p + p;
            return Frac(p);
        }

        // Integer Hash - II
        // - Inigo Quilez, Integer Hash - II, 2017
        //   https://www.shadertoy.com/view/XlXcW4
        static const uvec3 iqint2(uvec3 x)
        {
            const uint k = 1103515245u;

            x = ((x >> 8u) ^ uvec3(x.y, x.z, x.x)) * k;
            x = ((x >> 8u) ^ uvec3(x.y, x.z, x.x)) * k;
            x = ((x >> 8u) ^ uvec3(x.y, x.z, x.x)) * k;

            return x;
        }

        // http://www.jcgt.org/published/0009/03/02/
        static const uvec3 pcg3d(uvec3 v)
        {
            v = v * 1664525u + 1013904223u;

            v.x += v.y * v.z;
            v.y += v.z * v.x;
            v.z += v.x * v.y;

            v = v ^ (v >> 16u);

            v.x += v.y * v.z;
            v.y += v.z * v.x;
            v.z += v.x * v.y;

            return v;
        }

        // SuperFastHash, adapated from http://www.azillionmonkeys.com/qed/hash.html
        static const uint superfast(const uvec3 data)
        {
            uint hash = 8u, tmp;

            hash += data.x & 0xffffu;
            tmp = (((data.x >> 16) & 0xffffu) << 11) ^ hash;
            hash = (hash << 16) ^ tmp;
            hash += hash >> 11;

            hash += data.y & 0xffffu;
            tmp = (((data.y >> 16) & 0xffffu) << 11) ^ hash;
            hash = (hash << 16) ^ tmp;
            hash += hash >> 11;

            hash += data.z & 0xffffu;
            tmp = (((data.z >> 16) & 0xffffu) << 11) ^ hash;
            hash = (hash << 16) ^ tmp;
            hash += hash >> 11;

            /* Force "avalanching" of final 127 bits */
            hash ^= hash << 3;
            hash += hash >> 5;
            hash ^= hash << 4;
            hash += hash >> 17;
            hash ^= hash << 25;
            hash += hash >> 6;

            return hash;
        }

        // Used to switch hash methods easily for testing
        // pcg3d seems to be the best
        static const uvec3 Hash(const uvec3& v)
        {
            //return uvec3(superfast(v));
            //return iqint2(v);
            return pcg3d(v);
        }

        // Turns a hashed uvec3 into a noise value
        static const double HashToNoise(const uvec3 hash)
        {
            return (double(hash.y % 65536u) / 65536.0);
        }
    };
}
