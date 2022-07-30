#include "RayReceiver.h"
#include "CommonHeader.h"
#include "Ray.h"
#include "AABB.h"

namespace gfx
{
    class IceSurface : public RayReceiver
    {
    public:
        IceSurface(const vec3f positionWS, std::shared_ptr<Material> pMaterial);
    public:
        virtual const bool Hit(const Ray& r, const f32 t_min, const f32 t_max, RayHitRecord& rec, const uint gBufferIdx) const override;
        virtual const bool GetAABB(AABB& aabb) const override;
        void GetSurfaceInfo(const vec3f& p, f32& rayHeightAboveSurface, f32& isIceSurface) const;
        static vec3f GetSurfaceNormal(const vec3f& p, const f32 isIceSurface);
    private:
        const bool GetRayPlaneHit(f32& t) const;
        static f32 GetWaterLevel(const vec3f& p);
        static f32 GetWaterLevel_Detailed(const vec3f& p);
        static vec3f GetWaterNormal(const vec3f& p);
    public:
        std::shared_ptr<Material> m_pMaterial;
    };
}