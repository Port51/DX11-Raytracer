#pragma once
#define NOMINMAX

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
		float materialSubIndex;
		std::shared_ptr<Material> pMaterial;
	};

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

#include "MathUtils.h"