#include "RayReceiver.h"
#include "CommonHeader.h"
#include "Ray.h"
#include "AABB.h"

namespace gfx
{
	class SphereObject : public RayReceiver
	{
    public:
        SphereObject(const vec3 positionWS, const double radius, std::shared_ptr<Material> pMaterial);
    public:
        virtual const bool Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec) const override;
        virtual const bool GetAABB(AABB& aabb) const override
        {
            const auto radiusVec = vec3(m_radius, m_radius, m_radius);
            AABB box0(m_prevFramePositionWS - radiusVec, m_prevFramePositionWS + radiusVec);
            AABB box1(m_positionWS - radiusVec, m_positionWS + radiusVec);
            aabb = AABB::GetCombinedAABB(box0, box1);
            return true;
        }
        static void GetHitUV(const vec3& p, double& u, double& v)
        {
            const auto theta = std::acos(-p.y);
            const auto phi = std::atan2(-p.z, p.x) + PI;

            u = phi / (2.0 * PI);
            v = theta / PI;
        }
    public:
        double m_radius;
        std::shared_ptr<Material> m_pMaterial;
	};
}