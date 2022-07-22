#pragma once
#include "CommonHeader.h"

namespace gfx
{
    class PerlinNoise
    {
    private:
        static const uint SeedCt = 256u; // must be power of 2!
        static const uint SeedCtX = 16u; // must be power of 2!
        static const uint SeedCtY = 16u; // must be power of 2!
        static const uint SeedCtZ = 8u; // must be power of 2!
    public:
        PerlinNoise()
        {
            m_RandomSeeds.resize(SeedCt);
            //m_RandomSeeds.resize(SeedCtX * SeedCtY * SeedCtZ);
            for (int i = 0; i < m_RandomSeeds.size(); ++i)
            {
                m_RandomSeeds[i] = Random::RandomDouble() * 2.0 - 1.0;
            }

            GetPermutations(m_PermutedX, 256);
            GetPermutations(m_PermutedY, 256);
            GetPermutations(m_PermutedZ, 256);
        }
        ~PerlinNoise() = default;

        const double GetNoise3D(const vec3& p, const uint octaveCt) const
        {

        }

        double Noise(const vec3& p, const uint octaveCt) const
        {
            const bool UseBilinearSampling = true;

            auto result = 0.0;
            for (uint o = 0; o < octaveCt; ++o)
            {
                const uint samples = 1u << o; // 2^o samples
                const auto scaleFactor = 1.0 / samples;
                const uint pitch = SeedCt >> (o + 1u);

                const int px = static_cast<int>(p.x);
                const int py = static_cast<int>(p.y);
                const int pz = static_cast<int>(p.z);
                //const uint px = ((int)p.x % SeedCtX + SeedCtX) % SeedCtX;
                //const uint py = ((int)p.y % SeedCtX + SeedCtX) % SeedCtX;
                //const uint pz = ((int)p.z % SeedCtZ + SeedCtZ) % SeedCtZ;

                // Reduce samples based on pitch
                const int x0 = (px / pitch) * pitch;
                const int y0 = (py / pitch) * pitch;
                const int z0 = (pz / pitch) * pitch;

                if (UseBilinearSampling)
                {
                    const int x1 = (x0 + pitch) % SeedCtX;
                    const int y1 = (y0 + pitch) % SeedCtY;
                    const int z1 = (z0 + pitch) % SeedCtZ;
                    const double xf = (p.x - x0) / pitch;
                    const double yf = (p.y - y0) / pitch;
                    const double zf = (p.z - z0) / pitch;

                    if (xf > 1.0 || yf > 1.0 || zf > 1.0)
                    {
                        auto bbb = 0;
                    }

                    const auto s000 = GetRandomSeed(x0, y0, z0);
                    const auto s100 = GetRandomSeed(x1, y0, z0);
                    const auto s110 = GetRandomSeed(x1, y1, z0);
                    const auto s111 = GetRandomSeed(x1, y1, z1);
                    const auto s011 = GetRandomSeed(x0, y1, z1);
                    const auto s001 = GetRandomSeed(x0, y0, z1);
                    const auto s101 = GetRandomSeed(x1, y0, z1);
                    const auto s010 = GetRandomSeed(x0, y1, z0);

                    const auto sample = Lerp(
                        Lerp(Lerp(s000, s001, zf), Lerp(s010, s011, zf), yf),
                        Lerp(Lerp(s100, s101, zf), Lerp(s110, s111, zf), yf),
                        xf);

                    result += sample * scaleFactor;
                }
                else
                {
                    const auto sample = GetRandomSeed(x0, y0, z0);
                    result += sample * scaleFactor;
                }
            }
            return result * 0.5 + 0.5;
        }

    private:
        const uint32_t hash(uint32_t a) const
        {
            a = (a + 0x7ed55d16) + (a << 12);
            a = (a ^ 0xc761c23c) ^ (a >> 19);
            a = (a + 0x165667b1) + (a << 5);
            a = (a + 0xd3a2646c) ^ (a << 9);
            a = (a + 0xfd7046c5) + (a << 3);
            a = (a ^ 0xb55a4f09) ^ (a >> 16);
            return a;
        }

        const uint32_t hash2(uint32_t a) const
        {
            a -= (a << 6);
            a ^= (a >> 17);
            a -= (a << 9);
            a ^= (a << 4);
            a -= (a << 3);
            a ^= (a << 10);
            a ^= (a >> 15);
            return a;
        }

        const double GetRandomSeed(const uint x, const uint y, const uint z) const
        {
            return m_RandomSeeds.at(hash2(x ^ y ^ z) & 255);
            //return m_RandomSeeds.at(m_PermutedX.at(x & 255) ^ m_PermutedX.at(y & 255) ^ m_PermutedX.at(z & 255));
            //return m_RandomSeeds.at((z * SeedCtY + y) * SeedCtX + x);
        }

        void GetPermutations(std::vector<int>& v, size_t count)
        {
            v.resize(count);
            for (size_t i = 0u; i < v.size(); ++i)
            {
                v[i] = i;
            }

            for (size_t i = 0u; i < v.size(); ++i)
            {
                size_t target = Random::RandomInt(i, v.size() - 1u);
                std::swap(v[i], v[target]);
            }
        }

    private:
        std::vector<double> m_RandomSeeds;
        std::vector<int> m_PermutedX;
        std::vector<int> m_PermutedY;
        std::vector<int> m_PermutedZ;
    };
}
