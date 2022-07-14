#include "RenderObject.h"
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
	class SphereObject : public RenderObject
	{
    public:
        SphereObject(const vec3 positionWS, const double radius, std::shared_ptr<Material> pMaterial);
    public:
        virtual bool Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec) const override;
    public:
        double m_radius;
        std::shared_ptr<Material> m_pMaterial;
	};
}