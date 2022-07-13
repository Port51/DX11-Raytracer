#include "SphereObject.h"

namespace gfx
{
    SphereObject::SphereObject(vec3 cen, double r)
        : center(cen), radius(r), RenderObject()
    {
    }

    bool SphereObject::Hit(const Ray& r, double t_min, double t_max, RayHitRecord& rec) const
	{
        vec3 oc = r.origin() - center;
        auto a = Dot(r.direction(), r.direction());
        auto half_b = Dot(oc, r.direction());
        auto c = Dot(oc, oc) - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0) return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (root < t_min || t_max < root)
        {
            root = (-half_b + sqrtd) / a;
            if (root < t_min || t_max < root)
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        
        // Handle inward facing normals
        vec3 normal = (rec.p - center) / radius;
        rec.isFrontFacing = Dot(r.direction(), normal) < 0;
        rec.normal = rec.isFrontFacing ? normal : -normal;

        return true;
	}
}