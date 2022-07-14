#pragma once

#include <random>

class Random
{
public:
	static double RandomDouble()
	{
		static std::uniform_real_distribution<double> distribution(0.0, 1.0);
		static std::mt19937 generator;
		return distribution(generator);
	}

	static double RandomDouble(const double min, const double max)
	{
		return RandomDouble() * (max - min) + min;
	}

	static float RandomFloat()
	{
		static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		static std::mt19937 generator;
		return distribution(generator);
	}

	static float RandomFloat(const float min, const float max)
	{
		return RandomFloat() * (max - min) + min;
	}
};
