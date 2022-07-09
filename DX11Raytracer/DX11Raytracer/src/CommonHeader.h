#pragma once

const int ScreenWidth = 256;
const int ScreenHeight = 256;
const float AspectRatio = (float)ScreenWidth / ScreenHeight;
const int TileSize = 16u;
const int TileDimensionX = ScreenWidth / TileSize;
const int TileDimensionY = ScreenHeight / TileSize;

struct RGBA
{
	float r;
	float g;
	float b;
	float a;
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
