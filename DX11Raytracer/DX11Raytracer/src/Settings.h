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

	static const bool UseHQNoise = true;
	static const bool UseRaymarchSlices = false;
	static const double RaymarchDensity = 0.1;
	static const double MaxRaymarchDistance = 8.0;
	static const uint32_t RaymarchPassCt = 2u;
	static const uint32_t RaymarchStepsPerPass = 100u;
	static const uint32_t RaymarchHeightFieldTraceSteps = 50u;
}