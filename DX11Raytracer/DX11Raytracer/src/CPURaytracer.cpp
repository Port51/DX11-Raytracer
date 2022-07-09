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
        auto viewport_height = 2.0;
        auto viewport_width = AspectRatio * viewport_height;
        auto focal_length = 1.0;

        auto origin = vec3(0, 0, 0);
        auto horizontal = vec3(viewport_width, 0, 0);
        auto vertical = vec3(0, viewport_height, 0);
        auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);


        for (int lx = 0; lx < TileSize; ++lx)
        {
            for (int ly = 0; ly < TileSize; ++ly)
            {
                int x = TileSize * tileX + lx;
                int y = TileSize * tileY + ly;

                double u = static_cast<double>(x) / (ScreenWidth - 1);
                double v = static_cast<double>(y) / (ScreenHeight - 1);

                int localIdx = ly * TileSize + lx + tileOffset;

                Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
                GetRayColor(r, &buffer[localIdx]);
            }
        }

	}

    void CPURaytracer::GetRayColor(Ray& ray, RGBA* const bufferSegment) const
    {
        // Sky background
        vec3 unit_direction = unit_vector(ray.direction());
        auto t = 0.5 * (unit_direction.y() + 1.0);
        auto c = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);

        vec3 hitPt;
        if (HitSphere(ray, hitPt))
        {
            c[0] = 1;
            c[1] = 0;
            c[2] = 0;

            // Normal
            vec3 n = unit_vector(hitPt - vec3(0, 0, -1));
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
        vec3 spherePos = vec3(0, 0, -1);
        double rad = 0.5;

        // Test if in sphere
        vec3 displ = spherePos - ray.orig;
        double dSqr = dot(displ, displ);
        bool inSphere = (dSqr <= rad * rad);

        double tPlane = dot(displ, ray.dir); // projection of displ onto dir, note that |dir| == 1

        if (tPlane > 0)
        {
            // Find collision pt on plane and test against radius
            vec3 planeCollisionPt = ray.orig + ray.dir * tPlane;
            vec3 collisionDispl = spherePos - planeCollisionPt;
            double cdSqr = dot(collisionDispl, collisionDispl);

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
}