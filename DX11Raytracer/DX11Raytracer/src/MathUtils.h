#pragma once
#include <cmath>
#include <algorithm>

namespace gfx
{
	// Constants

	static const float Infinity_F = std::numeric_limits<float>::infinity();
	static const double Infinity = std::numeric_limits<double>::infinity();
	static const float PI_F = 3.1415926535897932385f;
	static const double PI = 3.1415926535897932385;

	// Utility Functions

	inline float DegreesToRadians(const float degrees)
	{
		return degrees * PI_F / 180.0f;
	}

	inline double DegreesToRadians(const double degrees)
	{
		return degrees * PI / 180.0;
	}

	inline float Clamp(const float x, const float _min, const float _max)
	{
		return std::max(std::min(x, _max), _min);
	}

	inline double Clamp(const double x, const double _min, const double _max)
	{
		return std::max(std::min(x, _max), _min);
	}

	inline float Saturate(const float x)
	{
		return std::max(std::min(x, 1.0f), 0.0f);
	}

	inline double Saturate(const double x)
	{
		return std::max(std::min(x, 1.0), 0.0);
	}

	inline float Frac(const float x)
	{
		const float whole = std::floor(x);
		return x - whole;
	}

	inline double Frac(const double x)
	{
		const double whole = std::floor(x);
		return x - whole;
	}

	inline float Lerp(const float a, const float b, const float lerp)
	{
		return a * (1.0f - lerp) + b * lerp;
	}

	inline double Lerp(const double a, const double b, const double lerp)
	{
		return a * (1.0 - lerp) + b * lerp;
	}

	inline float SCurve(const float v)
	{
		return v * v * (3.0f - 2.0f * v);
	}

	inline double SCurve(const double v)
	{
		return v * v * (3.0 - 2.0 * v);
	}

}