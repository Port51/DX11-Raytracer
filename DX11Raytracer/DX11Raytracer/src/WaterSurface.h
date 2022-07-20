#include "RayReceiver.h"
#include "CommonHeader.h"
#include "Ray.h"
#include "AABB.h"

namespace gfx
{
    class WaterSurface : public RayReceiver
    {
    public:
        WaterSurface(const vec3 positionWS, std::shared_ptr<Material> pMaterial);
    public:
        virtual const bool Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec) const override;
        virtual const bool GetAABB(AABB& aabb) const override;
        static double GetRayHeightAboveOcean(const vec3 p);
        static vec3 GetOceanNormal(const vec3 p);
    private:
        const bool GetRayPlaneHit(double& t) const;
    public:
        std::shared_ptr<Material> m_pMaterial;
    };
}