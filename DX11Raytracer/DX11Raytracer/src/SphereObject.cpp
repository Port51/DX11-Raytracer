#include "SphereObject.h"

namespace gfx
{
    SphereObject::SphereObject(vec3 cen, double r, std::shared_ptr<Material> pMaterial)
        : m_center(cen), m_radius(r), m_pMaterial(std::move(pMaterial)), RenderObject()
    {
    }

    bool SphereObject::Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec) const
	{
        vec3 oc = r.GetOrigin() - m_center;
        auto a = Dot(r.GetDirection(), r.GetDirection());
        auto half_b = Dot(oc, r.GetDirection());
        auto c = Dot(oc, oc) - m_radius * m_radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.GetPositionAfterTime(rec.t);
        
        // Handle inward facing normals
        vec3 normal = (rec.p - m_center) / m_radius;
        rec.isFrontFacing = Dot(r.GetDirection(), normal) < 0;
        rec.normal = rec.isFrontFacing ? normal : -normal;
        rec.pMaterial = m_pMaterial;

        return true;
	}
}