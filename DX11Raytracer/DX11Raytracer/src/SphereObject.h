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
        virtual const bool GetAABB(AABB& aabb) const override;
        static void GetHitUV(const vec3& p, double& u, double& v);
    public:
        double m_radius;
        std::shared_ptr<Material> m_pMaterial;
	};
}