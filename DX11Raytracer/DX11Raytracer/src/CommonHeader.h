#pragma once
#include "Random.h"
#include "vec3.h"
#include "uvec3.h"
#include "ivec3.h"
#include "uvec3.h"
#include "Color.h"
#include <cmath>
#include <limits>
#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

namespace gfx
{

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
		return max(min(x, _max), _min);
	}

	inline double Clamp(const double x, const double _min, const double _max)
	{
		return max(min(x, _max), _min);
	}

	inline float Saturate(const float x)
	{
		return max(min(x, 1.0f), 0.0f);
	}

	inline double Saturate(const double x)
	{
		return max(min(x, 1.0), 0.0);
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