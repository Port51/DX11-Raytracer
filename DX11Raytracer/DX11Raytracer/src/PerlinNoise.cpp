#include "PerlinNoise.h"
#include "Settings.h"

namespace gfx
{
    const double PerlinNoise::GetNoise3D(const vec3& pd, const uint octaveCt)
    {
        vec3f p = vec3f(static_cast<float>(pd.x) + 1000000.0f, static_cast<float>(pd.y) + 1000000.0f, static_cast<float>(pd.z) + 1000000.0f);
        const uvec3 u = uvec3(p.x, p.y, p.z);

        float noise = 0.0;
        float scale = 1.0;
        uint pitch = 64u;
        for (uint i = 0; i < octaveCt; ++i)
        {
            const uvec3 u2 = u - (u % uvec3(pitch));

            // Bilinear sampling
            const vec3f lerp = SCurve((p - vec3f(u2.x, u2.y, u2.z)) / static_cast<float>(pitch));

            const float s000 = Hash1DToNoise(Hash3D(u2.x, u2.y, u2.z));
            const float s100 = Hash1DToNoise(Hash3D(u2.x + pitch, u2.y, u2.z));
            const float s010 = Hash1DToNoise(Hash3D(u2.x, u2.y + pitch, u2.z));
            const float s110 = Hash1DToNoise(Hash3D(u2.x + pitch, u2.y + pitch, u2.z));
            const float s001 = Hash1DToNoise(Hash3D(u2.x, u2.y, u2.z + pitch));
            const float s101 = Hash1DToNoise(Hash3D(u2.x + pitch, u2.y, u2.z + pitch));
            const float s011 = Hash1DToNoise(Hash3D(u2.x, u2.y + pitch, u2.z + pitch));
            const float s111 = Hash1DToNoise(Hash3D(u2.x + pitch, u2.y + pitch, u2.z + pitch));

            noise += scale * Lerp(
                Lerp(Lerp(s000, s100, lerp.x), Lerp(s010, s110, lerp.x), lerp.y),
                Lerp(Lerp(s001, s101, lerp.x), Lerp(s011, s111, lerp.x), lerp.y), lerp.z);

            scale *= 0.5f;
            pitch /= 2u;
        }

        return static_cast<double>(noise);
    }

    // Hash without Sine
    // https://www.shadertoy.com/view/4djSRW

    const float PerlinNoise::HashWithoutSine11(float p)
    {
        p = Frac(p * 0.1031f);
        p *= p + 33.33f;
        p *= p + p;
        return Frac(p);
    }

    // Integer Hash - II
    // - Inigo Quilez, Integer Hash - II, 2017
    //   https://www.shadertoy.com/view/XlXcW4

    const uvec3 PerlinNoise::IQint2(uvec3 x)
    {
        const uint k = 1103515245u;

        x = ((x >> 8u) ^ uvec3(x.y, x.z, x.x)) * k;
        x = ((x >> 8u) ^ uvec3(x.y, x.z, x.x)) * k;
        x = ((x >> 8u) ^ uvec3(x.y, x.z, x.x)) * k;

        return x;
    }

    // http://www.jcgt.org/published/0009/03/02/

    const uvec3 PerlinNoise::PCG_3D(uvec3 v)
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
    // Uses fewer instructions and doesn't copy a vec3
    const uint PerlinNoise::PCG_3D_to_1D(uint x, uint y, uint z)
    {
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

    // Adapted from https://www.shadertoy.com/view/XlGcRh
    const uint PerlinNoise::xxHash32(const uint x, const uint y, const uint z)
    {
        const uint PRIME32_2 = 2246822519u, PRIME32_3 = 3266489917u;
        const uint PRIME32_4 = 668265263u, PRIME32_5 = 374761393u;
        uint h32 = z + PRIME32_5 + x * PRIME32_3;
        h32 = PRIME32_4 * ((h32 << 17u) | (h32 >> (32u - 17u)));
        h32 += y * PRIME32_3;
        h32 = PRIME32_4 * ((h32 << 17u) | (h32 >> (32u - 17u)));
        h32 = PRIME32_2 * (h32 ^ (h32 >> 15u));
        h32 = PRIME32_3 * (h32 ^ (h32 >> 13u));
        return h32 ^ (h32 >> 16u);
    }

    const inline uint PerlinNoise::Hash3D(const uint x, const uint y, const uint z)
    {
        return UseHQNoise ? PCG_3D_to_1D(x, y, z) : xxHash32(x, y, z);
    }

    // SuperFastHash, adapated from http://www.azillionmonkeys.com/qed/hash.html

    const uint PerlinNoise::SuperFast(const uvec3 data)
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

    // Turns a hashed uvec3 into a noise value

    inline const float PerlinNoise::Hash3DToNoise(const uvec3& hash)
    {
        return (static_cast<float>(hash.y % 65536u) / 65536.0f);
    }

    // Turns a hashed uint into a noise value

    inline const float PerlinNoise::Hash1DToNoise(const uint hash)
    {
        return (static_cast<float>(hash % 65536u) / 65536.0f);
    }
}