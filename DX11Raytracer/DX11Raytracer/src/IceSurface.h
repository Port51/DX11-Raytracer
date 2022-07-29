#include "RayReceiver.h"
#include "CommonHeader.h"
#include "Ray.h"
#include "AABB.h"

namespace gfx
{
    class IceSurface : public RayReceiver
    {
    public:
        IceSurface(const vec3 positionWS, std::shared_ptr<Material> pMaterial);
    public:
        virtual const bool Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec, const uint gBufferIdx) const override;
        virtual const bool GetAABB(AABB& aabb) const override;
        void GetSurfaceInfo(const vec3 p, double& rayHeightAboveSurface, float& isIceSurface) const;
        static vec3 GetSurfaceNormal(const vec3 p, const bool isIceSurface);
    private:
        const bool GetRayPlaneHit(double& t) const;
    public:
        std::shared_ptr<Material> m_pMaterial;
    };
}