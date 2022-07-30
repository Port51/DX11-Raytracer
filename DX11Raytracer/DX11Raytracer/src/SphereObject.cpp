#include "SphereObject.h"
#include "Material.h"

namespace gfx
{
    SphereObject::SphereObject(const vec3f positionWS, const f32 radius, std::shared_ptr<Material> pMaterial)
        : m_radius(radius), m_pMaterial(std::move(pMaterial)), RayReceiver(positionWS)
    {
    }

    const bool SphereObject::Hit(const Ray& r, const f32 tMin, const f32 tMax, RayHitRecord& rec, const uint gBufferIdx) const
	{
        // Filter by GBuffer
        if (!m_pMaterial->IsInGBuffer(gBufferIdx)) return false;

        vec3f center = GetPositionAtTime(r.GetTime());
        vec3f oc = r.GetOrigin() - center;
        f32 a = Dot(r.GetDirection(), r.GetDirection());
        f32 halfB = Dot(oc, r.GetDirection());
        f32 c = Dot(oc, oc) - m_radius * m_radius;

        f32 discriminant = halfB * halfB - a * c;
        if (discriminant < 0) return false;
        f32 sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        f32 root = (-halfB - sqrtd) / a;
        if (root < tMin || tMax < root)
        {
            root = (-halfB + sqrtd) / a;
            if (root < tMin || tMax < root)
                return false;
        }

        rec.time = root;
        rec.positionWS = r.GetPositionAfterTime(rec.time);
        
        // Handle inward facing normals
        vec3f normal = (rec.positionWS - center) / m_radius;
        rec.isFrontFacing = Dot(r.GetDirection(), normal) < 0;
        rec.normalWS = rec.isFrontFacing ? normal : -normal;
        rec.pMaterial = m_pMaterial;
        GetHitUV(normal, rec.u, rec.v);

        return true;
	}

    const bool SphereObject::GetAABB(AABB& aabb) const
    {
        const vec3f radiusVec = vec3f(m_radius, m_radius, m_radius);
        AABB box0(m_prevFramePositionWS - radiusVec, m_prevFramePositionWS + radiusVec);
        AABB box1(m_positionWS - radiusVec, m_positionWS + radiusVec);
        aabb = AABB::GetCombinedAABB(box0, box1);
        return true;
    }

    void SphereObject::GetHitUV(const vec3f& p, f32& u, f32& v)
    {
        const f32 theta = std::acos(-p.y);
        const f32 phi = std::atan2(-p.z, p.x) + PI;

        u = phi / (2.0f * PI);
        v = theta / PI;
    }
}