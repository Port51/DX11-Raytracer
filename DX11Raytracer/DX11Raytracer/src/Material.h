#pragma once
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
    struct RayHitRecord;

    class Material
    {
    public:
        virtual bool Scatter(const Ray& r_in, const RayHitRecord& rec, Color& attenuation, Ray& scattered) const = 0;
    };
}