#pragma once
#include "CommonHeader.h"
#include "vec3f.h"

namespace gfx
{
    class PerlinNoise
    {
    public:
        PerlinNoise() = default;
        ~PerlinNoise() = default;

        static const double GetNoise3D(const vec3& pd, const uint octaveCt);

    private:
        // Hash without Sine
        // https://www.shadertoy.com/view/4djSRW
        static const float HashWithoutSine11(float p);

        // Integer Hash - II
        // - Inigo Quilez, Integer Hash - II, 2017
        //   https://www.shadertoy.com/view/XlXcW4
        static const uvec3 IQint2(uvec3 x);

        // http://www.jcgt.org/published/0009/03/02/
        static const uvec3 PCG_3D(uvec3 v);

        // Modified from http://www.jcgt.org/published/0009/03/02/
        // Uses fewer instructions and doesn't copy a vec3
        static const uint PCG_3D_to_1D(uint x, uint y, uint z);

        static const uint xxHash32(const uint x, const uint y, const uint z);

        static const uint Hash3D(const uint x, const uint y, const uint z);

        // SuperFastHash, adapated from http://www.azillionmonkeys.com/qed/hash.html
        static const uint SuperFast(const uvec3 data);

        // Turns a hashed uvec3 into a noise value
        static inline const float Hash3DToNoise(const uvec3& hash);

        // Turns a hashed uint into a noise value
        static inline const float Hash1DToNoise(const uint hash);

    };
}
