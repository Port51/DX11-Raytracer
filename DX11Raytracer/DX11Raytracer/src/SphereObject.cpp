#include "SphereObject.h"

namespace gfx
{
    SphereObject::SphereObject(const vec3 positionWS, const double radius, std::shared_ptr<Material> pMaterial)
        : m_radius(radius), m_pMaterial(std::move(pMaterial)), RayReceiver(positionWS)
    {
    }

    const bool SphereObject::Hit(const Ray& r, const double tMin, const double tMax, RayHitRecord& rec) const
	{
        vec3 center = GetPositionAtTime(r.GetTime());
        vec3 oc = r.GetOrigin() - center;
        auto a = Dot(r.GetDirection(), r.GetDirection());
        auto halfB = Dot(oc, r.GetDirection());
        auto c = Dot(oc, oc) - m_radius * m_radius;

        auto discriminant = halfB * halfB - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-halfB - sqrtd) / a;
        if (root < tMin || tMax < root)
        {
            root = (-halfB + sqrtd) / a;
            if (root < tMin || tMax < root)
                return false;
        }

        rec.time = root;
        rec.positionWS = r.GetPositionAfterTime(rec.time);
        
        // Handle inward facing normals
        vec3 normal = (rec.positionWS - center) / m_radius;
        rec.isFrontFacing = Dot(r.GetDirection(), normal) < 0;
        rec.normalWS = rec.isFrontFacing ? normal : -normal;
        rec.pMaterial = m_pMaterial;
        GetHitUV(normal, rec.u, rec.v);

        return true;
	}
}