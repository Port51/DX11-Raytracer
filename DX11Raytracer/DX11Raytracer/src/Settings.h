#pragma once
#include <cmath>

namespace gfx
{
	static const int ScreenWidth = 512;
	static const int ScreenHeight = 256;
	static const float AspectRatio = (float)ScreenWidth / ScreenHeight;
	static const unsigned int TileSize = 16u;
	static const unsigned int TileDimensionX = ScreenWidth / TileSize;
	static const unsigned int TileDimensionY = ScreenHeight / TileSize;
	static const unsigned int TileCount = TileDimensionX * TileDimensionY;

	static const bool UseThreading = true;
	static const bool UseDepthOfField = true;

	static const uint32_t RaymarchPassCt = 20u;
	static const uint32_t RaymarchStepsPerPass = 50u;
	static const uint32_t RaymarchHeightFieldTraceSteps = 50u;
}