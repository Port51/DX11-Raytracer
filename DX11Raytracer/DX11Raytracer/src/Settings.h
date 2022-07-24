#pragma once
#include <cmath>

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

	const uint32_t RaymarchPassCt = 20u;
}