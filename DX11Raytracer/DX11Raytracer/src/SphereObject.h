#include "RayReceiver.h"
#include "CommonHeader.h"
#include "Ray.h"
#include "AABB.h"

namespace gfx
{
	class SphereObject : public RayReceiver
	{
    public:
        SphereObject(const vec3f positionWS, const f32 radius, std::shared_ptr<Material> pMaterial);
    public:
        virtual const bool Hit(const Ray& r, const f32 t_min, const f32 t_max, RayHitRecord& rec, const uint gBufferIdx) const override;
        virtual const bool GetAABB(AABB& aabb) const override;
        static void GetHitUV(const vec3f& p, f32& u, f32& v);
    public:
        f32 m_radius;
        std::shared_ptr<Material> m_pMaterial;
	};
}