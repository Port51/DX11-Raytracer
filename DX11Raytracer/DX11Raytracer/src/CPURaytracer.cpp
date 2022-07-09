#include "CPURaytracer.h"
#include "Ray.h"

namespace gfx
{
	CPURaytracer::CPURaytracer()
	{}

	void CPURaytracer::RunTile(RGBA* const buffer, const uint tileX, const uint tileY) const
	{
		const int tileOffset = (tileY * TileDimensionX + tileX) * (TileSize * TileSize);

        // todo: move this to camera class
        /*auto viewport_height = 2.0;
        auto viewport_width = AspectRatio * viewport_height;
        auto focal_length = 1.0;

        // Setup frustum corners using near plane
        auto horizontal = vec3(viewport_width, 0, 0);
        auto vertical = vec3(0, viewport_height, 0);
        auto lower_left_corner = cameraPositionWS - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);*/

        auto vFov = 65.0 * 3.1415 / 180.0;
        auto halfAngleY = std::tan(vFov * 0.5);
        auto halfAngleX = halfAngleY * AspectRatio;
        vec3 frustumCornerVS = vec3(halfAngleX, halfAngleY, 1.0);

        auto cameraPositionWS = vec3(0, 0, 0);

        // todo: transform this by view matrix

        for (int lx = 0; lx < TileSize; ++lx)
        {
            for (int ly = 0; ly < TileSize; ++ly)
            {
                int x = TileSize * tileX + lx;
                int y = TileSize * tileY + ly;

                // NDC coords
                double u = static_cast<double>(x) / (ScreenWidth - 1) * 2.0 - 1.0;
                double v = static_cast<double>(y) / (ScreenHeight - 1) * 2.0 - 1.0;

                int localIdx = ly * TileSize + lx + tileOffset;

                vec3 dir = Normalize(vec3(u * frustumCornerVS.x(), v * frustumCornerVS.y(), -1.0));
                Ray r(cameraPositionWS, dir);
                GetRayColor(r, &buffer[localIdx]);
            }
        }

	}

    void CPURaytracer::GetRayColor(Ray& ray, RGBA* const bufferSegment) const
    {
        // Sky background
        vec3 unit_direction = Normalize(ray.direction());
        auto t = 0.5 * (unit_direction.y() + 1.0);
        auto c = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);

        vec3 hitPt;
        /*if (HitSphere(ray, hitPt))
        {
            c[0] = 1;
            c[1] = 0;
            c[2] = 0;

            // Normal
            vec3 n = Normalize(hitPt - vec3(0, 0, 1));
            c[0] = n[0];
            c[1] = n[1];
            c[2] = n[2];

            // Simple lambert lighting from light
            auto v = -Dot(n, Normalize(vec3(0.0, -1.0, 0.0)));
            v = std::max(v, 0.0);
            c[0] = v;
            c[1] = v;
            c[2] = v;
        }*/

        t = hit_sphere(vec3(0, 0, -1), 0.5, ray);
        if (t >= 0.0)
        {
            vec3 n = Normalize(ray.at(t) - vec3(0, 0, -1)) * 0.5 + vec3(0.5);

            c[0] = n[0];
            c[1] = n[1];
            c[2] = n[2];
        }

        bufferSegment->r = c.x();
        bufferSegment->g = c.y();
        bufferSegment->b = c.z();
        bufferSegment->a = 0;
    }

    const bool CPURaytracer::HitSphere(const Ray& ray, vec3& hitPoint) const
    {
        // todo: move this
        vec3 spherePos = vec3(0, 0, 1);
        double rad = 0.5;

        // Test if in sphere
        vec3 displ = spherePos - ray.orig;
        double dSqr = Dot(displ, displ);
        bool inSphere = (dSqr <= rad * rad);

        double tPlane = Dot(displ, ray.dir); // projection of displ onto dir, note that |dir| == 1

        if (tPlane > 0)
        {
            // Find collision pt on plane and test against radius
            vec3 planeCollisionPt = ray.orig + ray.dir * tPlane;
            vec3 collisionDispl = spherePos - planeCollisionPt;
            double cdSqr = Dot(collisionDispl, collisionDispl);

            // Alt version w/ pythagorean
            //cdSqr = dSqr - tPlane * tPlane;

            if (cdSqr <= rad * rad)
            {
                // Now move forward or backwards to find collision point
                double offset = std::sqrt(rad * rad - cdSqr);
                double tSurface = tPlane + (inSphere) ? offset : -offset;

                hitPoint = ray.orig + ray.dir * tSurface;
                return true;
            }
        }
        return false;
    }

    const double CPURaytracer::hit_sphere(const vec3& center, double radius, const Ray& r) const
    {
        vec3 oc = r.origin() - center;
        auto a = Dot(r.direction(), r.direction());
        auto b = 2.0 * Dot(oc, r.direction());
        auto c = Dot(oc, oc) - radius * radius;
        auto discriminant = b * b - 4 * a * c;

        if (std::abs(a) < 0.00001) a = 0.00001;

        // Return distance, or -1
        if (discriminant < 0.0) return -1.0;
        else return (-b - std::sqrt(discriminant)) / (2.0 * a);
    }
}