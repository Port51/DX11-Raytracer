#include "RenderObject.h"
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
	class SphereObject : public RenderObject
	{
    public:
        SphereObject(vec3 cen, double r, std::shared_ptr<Material> pMaterial);
    public:
        virtual bool Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec) const override;
    public:
        vec3 m_center;
        double m_radius;
        std::shared_ptr<Material> m_pMaterial;
	};
}