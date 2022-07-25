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

            double v = 0.0;
            double scale = 1.0;
            uint pitch = 64u;
            for (uint i = 0; i < octaveCt; ++i)
            {
                const uvec3 u2 = u - (u % uvec3(pitch));
                const vec3 lerp = SCurve((p - vec3(u2.x, u2.y, u2.z)) / pitch);

                const double s000 = Hash1DToNoise(Hash(u2));
                const double s100 = Hash1DToNoise(Hash(u2 + uvec3(pitch, 0u, 0u)));
                const double s010 = Hash1DToNoise(Hash(u2 + uvec3(0u, pitch, 0u)));
                const double s110 = Hash1DToNoise(Hash(u2 + uvec3(pitch, pitch, 0u)));
                const double s001 = Hash1DToNoise(Hash(u2 + uvec3(0u, 0u, pitch)));
                const double s101 = Hash1DToNoise(Hash(u2 + uvec3(pitch, 0u, pitch)));
                const double s011 = Hash1DToNoise(Hash(u2 + uvec3(0u, pitch, pitch)));
                const double s111 = Hash1DToNoise(Hash(u2 + uvec3(pitch, pitch, pitch)));

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
            p = Frac(p * 0.1031f);
            p *= p + 33.33f;
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

        // Modified from http://www.jcgt.org/published/0009/03/02/
        static const uint pcg3d_to_1d(const uvec3& v)
        {
            uint x = v.x;
            uint y = v.y;
            uint z = v.z;
            x = x * 1664525u + 1013904223u;
            y = y * 1664525u + 1013904223u;
            z = z * 1664525u + 1013904223u;

            x += y * z;
            y += z * x;
            z += x * y;

            x = x ^ (x >> 16u);
            y = y ^ (y >> 16u);
            z = z ^ (z >> 16u);

            return (y * z + x) * z + y; // x2 [MAD] ops - can be reduced to x1 [MAD] op, but the scene looks way better this way
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
        static const uint Hash(const uvec3& v)
        {
            return pcg3d_to_1d(v);
        }

        // Turns a hashed uvec3 into a noise value
        static const double Hash3DToNoise(const uvec3 hash)
        {
            return (double(hash.y % 65536u) / 65536.0);
        }

        // Turns a hashed uint into a noise value
        static const double Hash1DToNoise(const uint hash)
        {
            return (double(hash % 65536u) / 65536.0);
        }

    };
}
