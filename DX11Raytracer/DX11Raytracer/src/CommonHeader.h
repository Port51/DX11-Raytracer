#pragma once
#include "Random.h"
#include "vec3.h"
#include "Color.h"
#include <cmath>
#include <limits>
#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

namespace gfx
{
	const int ScreenWidth = 512;
	const int ScreenHeight = 256;
	const float AspectRatio = (float)ScreenWidth / ScreenHeight;
	const unsigned int TileSize = 16u;
	const unsigned int TileDimensionX = ScreenWidth / TileSize;
	const unsigned int TileDimensionY = ScreenHeight / TileSize;
	const unsigned int TileCount = TileDimensionX * TileDimensionY;

	const bool UseThreading = true;
	const bool UseDepthOfField = true;

	class Material;
	struct RayHitRecord
	{
		vec3 positionWS;
		vec3 normalWS;
		double time;
		double u; // UV coords of hit
		double v;
		bool isFrontFacing;
		std::shared_ptr<Material> pMaterial;
	};

	// Constants

	const double Infinity = std::numeric_limits<double>::infinity();
	const double PI = 3.1415926535897932385;

	// Utility Functions

	inline double DegreesToRadians(const double degrees)
	{
		return degrees * PI / 180.0;
	}

	inline double Clamp(const double x, const double _min, const double _max)
	{
		return max(min(x, _max), _min);
	}

	inline double Frac(const double x)
	{
		const auto whole = std::floor(x);
		return x - whole;
	}

	inline double Lerp(const double a, const double b, const double lerp)
	{
		return a * (1.0 - lerp) + b * lerp;
	}

	#define ZERO_MEM(x) ZeroMemory(&x, sizeof(x))

	// Support fixed width types
	// https://en.cppreference.com/w/cpp/header/cstdint
	#include <cstdint>

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