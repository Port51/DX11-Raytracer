#pragma once

const int ScreenWidth = 256;
const int ScreenHeight = 256;
const float AspectRatio = (float)ScreenWidth / ScreenHeight;
const int TileSize = 16u;
const int TileDimensionX = ScreenWidth / TileSize;
const int TileDimensionY = ScreenHeight / TileSize;

#include <cmath>
#include <limits>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees)
{
	return degrees * pi / 180.0;
}

#include <random>

inline double random_double()
{
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

struct RGBA
{
	float r;
	float g;
	float b;
	float a;

	RGBA()
		: r(0.f), g(0.f), b(0.f), a(0.f)
	{}

	RGBA(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a)
	{}

	RGBA& operator+=(const RGBA& v)
	{
		r += v.r;
		g += v.g;
		b += v.b;
		a += v.a;
		return *this;
	}

	RGBA& operator*=(const double t)
	{
		r += t;
		g += t;
		b += t;
		a += t;
		return *this;
	}

	RGBA& operator/=(const double t)
	{
		return *this *= 1 / t;
	}
};

#define ZERO_MEM(x) ZeroMemory(&x, sizeof(x))

// Support fixed width types
// https://en.cppreference.com/w/cpp/header/cstdint
#include <cstdint>

namespace gfx
{
	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;
	using u8 = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;
	using f32 = float;
	using f64 = double;

	using guid64 = uint64_t;
	using slotUINT = uint8_t;

	using uint = unsigned int; // will be uint32_t and match UINT

	const guid64 NullGuid64 = 0u;
}

#include <memory>
#include "vec3.h"

#include <wrl.h>
using namespace Microsoft::WRL;
