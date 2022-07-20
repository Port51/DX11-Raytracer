#include "Material.h"

namespace gfx
{
    const Color gfx::Material::GetEmission(const RayHitRecord& rec) const
    {
        return Color(0.0, 0.0, 0.0, 0.0);
    }
}