#pragma once
#include <cmath>
#include <algorithm>

namespace gfx
{
	// Constants

	static const f32 Infinity_F = std::numeric_limits<f32>::infinity();
	static const f32 Infinity = std::numeric_limits<f32>::infinity();
	static const f32 PI = 3.1415926535897932385f;

	// Utility Functions

	inline f32 DegreesToRadians(const f32 degrees)
	{
		return degrees * PI / 180.f;
	}

	inline f32 Clamp(const f32 x, const f32 _min, const f32 _max)
	{
		return std::max(std::min(x, _max), _min);
	}

	inline f32 Saturate(const f32 x)
	{
		return std::max(std::min(x, 1.f), 0.f);
	}

	inline f32 Frac(const f32 x)
	{
		const f32 whole = std::floor(x);
		return x - whole;
	}

	inline f32 Lerp(const f32 a, const f32 b, const f32 lerp)
	{
		return a * (1.f - lerp) + b * lerp;
	}

	inline f32 SCurve(const f32 v)
	{
		return v * v * (3.f - 2.f * v);
	}

}