#pragma once
#include "CommonHeader.h"
#include "Ray.h"
#include "Color.h"

namespace gfx
{
    struct RayHitRecord;
    class GBuffer;

    class Material
    {
    public:
        virtual const bool Scatter(const Ray& r_in, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const GBuffer& gBuffer, const uint gBufferIdx) const = 0;
        virtual const bool IsInGBuffer(const uint gBufferIdx) const = 0;
    };
}