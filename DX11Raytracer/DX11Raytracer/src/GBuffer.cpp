#include "GBuffer.h"
#include "Settings.h"

namespace gfx
{
    GBuffer::GBuffer(const int screenWidth, const int screenHeight)
    {
        CameraColor.resize(static_cast<size_t>(screenWidth * screenHeight));
        IceRaymarchCache.resize(static_cast<size_t>(screenWidth * screenHeight));

        // Setup some test data to make sure tiles work correctly
        for (int tx = 0; tx < TileDimensionX; ++tx)
        {
            for (int ty = 0; ty < TileDimensionY; ++ty)
            {
                for (int lx = 0; lx < TileSize; ++lx)
                {
                    for (int ly = 0; ly < TileSize; ++ly)
                    {
                        int x = TileSize * tx + lx;
                        int y = TileSize * ty + ly;
                        f32 v0 = (f32)lx / TileSize;
                        f32 v1 = (f32)ly / TileSize;

                        int tileOffset = (TileSize * TileSize) * (ty * TileDimensionX + tx);
                        int pixelOffset = ly * TileSize + lx;
                        int idx = tileOffset + pixelOffset;

                        CameraColor[idx].r = v0;
                        CameraColor[idx].g = v1;
                        CameraColor[idx].b = (f32)x / 256;
                        CameraColor[idx].a = 0;

                        IceRaymarchCache[idx] = Color(0.f, 0.f, 0.f, 1.f);
                    }
                }
            }
        }
    }
}