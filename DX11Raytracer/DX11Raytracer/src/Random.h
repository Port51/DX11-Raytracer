#pragma once
#include "CommonHeader.h"
#include <random>

namespace gfx
{
	class Random
	{
	public:
		static f32 RandomFloat()
		{
			static std::uniform_real_distribution<f32> distribution(0.0f, 1.0f);
			static std::mt19937 generator;
			return distribution(generator);
		}

		static f32 RandomFloat(const f32 min, const f32 max)
		{
			return RandomFloat() * (max - min) + min;
		}

		static int RandomInt(const int min, const int max)
		{
			return static_cast<int>(RandomFloat(static_cast<f32>(min), static_cast<f32>(max + 1)));
		}
	};
}