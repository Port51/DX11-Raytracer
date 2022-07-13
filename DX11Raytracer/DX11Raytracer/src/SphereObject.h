#include "RenderObject.h"
#include "CommonHeader.h"
#include "Ray.h"

namespace gfx
{
	class SphereObject : public RenderObject
	{
    public:
        SphereObject(vec3 cen, double r);
    public:
        virtual bool Hit(const Ray& r, double t_min, double t_max, RayHitRecord& rec) const override;
    public:
        vec3 center;
        double radius;
	};
}