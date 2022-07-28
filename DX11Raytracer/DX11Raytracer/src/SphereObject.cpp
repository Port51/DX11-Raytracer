#include "SphereObject.h"
#include "Material.h"

namespace gfx
{
    SphereObject::SphereObject(const vec3 positionWS, const double radius, std::shared_ptr<Material> pMaterial)
        : m_radius(radius), m_pMaterial(std::move(pMaterial)), RayReceiver(positionWS)
    {
    }

    const bool SphereObject::Hit(const Ray& r, const double tMin, const double tMax, RayHitRecord& rec, const uint gBufferIdx) const
	{
        // Filter by GBuffer
        if (!m_pMaterial->IsInGBuffer(gBufferIdx)) return false;

        vec3 center = GetPositionAtTime(r.GetTime());
        vec3 oc = r.GetOrigin() - center;
        double a = Dot(r.GetDirection(), r.GetDirection());
        double halfB = Dot(oc, r.GetDirection());
        double c = Dot(oc, oc) - m_radius * m_radius;

        double discriminant = halfB * halfB - a * c;
        if (discriminant < 0) return false;
        double sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        double root = (-halfB - sqrtd) / a;
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

    const bool SphereObject::GetAABB(AABB& aabb) const
    {
        const vec3 radiusVec = vec3(m_radius, m_radius, m_radius);
        AABB box0(m_prevFramePositionWS - radiusVec, m_prevFramePositionWS + radiusVec);
        AABB box1(m_positionWS - radiusVec, m_positionWS + radiusVec);
        aabb = AABB::GetCombinedAABB(box0, box1);
        return true;
    }

    void SphereObject::GetHitUV(const vec3& p, double& u, double& v)
    {
        const double theta = std::acos(-p.y);
        const double phi = std::atan2(-p.z, p.x) + PI;

        u = phi / (2.0 * PI);
        v = theta / PI;
    }
}