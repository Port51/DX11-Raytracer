#pragma once
#include "Randomization.h"
#include "vec3.h"
#include "Color.h"
#include <cmath>
#include <limits>
#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

namespace gfx
{
	const int ScreenWidth = 256;
	const int ScreenHeight = 256;
	const float AspectRatio = (float)ScreenWidth / ScreenHeight;
	const int TileSize = 16u;
	const int TileDimensionX = ScreenWidth / TileSize;
	const int TileDimensionY = ScreenHeight / TileSize;

	// todo: move this!
	class Material;
	struct RayHitRecord
	{
		vec3 p;
		vec3 normal;
		double t;
		bool isFrontFacing;
		std::shared_ptr<Material> pMaterial;
	};

	// Constants

	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;

	// Utility Functions

	inline double degrees_to_radians(double degrees)
	{
		return degrees * pi / 180.0;
	}

	inline double clamp(double x, double min, double max)
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
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