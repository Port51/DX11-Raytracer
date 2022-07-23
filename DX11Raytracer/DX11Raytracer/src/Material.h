#pragma once
#include "CommonHeader.h"
#include "Ray.h"
#include "Color.h"

namespace gfx
{
    struct RayHitRecord;

    class Material
    {
    public:
        virtual const bool Scatter(const Ray& r_in, const RayHitRecord& rec, Color& attenuation, Color& emission, Ray& scattered, const uint bufferIdx) const = 0;
        virtual const Color GetEmission(const RayHitRecord& rec) const;
    };
}