#include "CPURaytracer.h"
#include "Ray.h"
#include "RenderObject.h"
#include "SphereObject.h"
#include "Lambertian.h"
#include "Metal.h"
#include "Dielectric.h"

namespace gfx
{
	CPURaytracer::CPURaytracer()
	{
        m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(0, 0, -1), 0.5, std::make_shared<Lambertian>(Color(1.0, 0.4, 0.4, 1.0))));
        m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(1, 0, -1), 0.5, std::make_shared<Metal>(Color(1.0, 0.4, 0.4, 1.0), 0.0)));
        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(-1, 0, -1), 0.5, std::make_shared<Metal>(Color(1.0, 1.0, 1.0, 1.0), 0.5)));
        m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(-1, 0, -1), -0.5, std::make_shared<Dielectric>(1.5)));
        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(-0.35, 0, -0.5), 0.27, std::make_shared<Dielectric>(1.5)));

        // Ground
        m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(0, -100.5, -1), 100, std::make_shared<Lambertian>(Color(0.1, 0.8, 0.2, 1.0))));
    }

	void CPURaytracer::RunTile(Color* const buffer, const uint tileX, const uint tileY) const
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

        auto vFov = 75.0 * 3.1415 / 180.0;
        auto halfAngleY = std::tan(vFov * 0.5);
        auto halfAngleX = halfAngleY * AspectRatio;
        vec3 frustumCornerVS = vec3(halfAngleX, halfAngleY, 1.0);

        auto cameraPositionWS = vec3(0, 0, 0.5);

        // todo: transform this by view matrix

        const int MaxBounces = 20;
        const int SamplesPerPixel = 11;
        const float AAScale = 1.f / SamplesPerPixel;
        //static double aaSamples[16] = { 0.0588235, 0.419608, 0.298039, 0.180392, 0.180392, 0.819608, 0.419608, 0.698039, 0.580392, 0.298039, 0.941176, 0.0588235, 0.698039, 0.941176, 0.819608, 0.580392 };

        for (int lx = 0; lx < TileSize; ++lx)
        {
            for (int ly = 0; ly < TileSize; ++ly)
            {
                int x = TileSize * tileX + lx;
                int y = TileSize * tileY + ly;
                int localIdx = ly * TileSize + lx + tileOffset;

                Color color;

                for (int a = 0; a < SamplesPerPixel; ++a)
                {
                    // NDC coords
                    double u = static_cast<double>(x + random_double()) / (ScreenWidth - 1) * 2.0 - 1.0;
                    double v = static_cast<double>(y + random_double()) / (ScreenHeight - 1) * 2.0 - 1.0;
                    //double u = static_cast<double>(x + aaSamples[a * 2 + 0]) / (ScreenWidth - 1) * 2.0 - 1.0;
                    //double v = static_cast<double>(y + aaSamples[a * 2 + 1]) / (ScreenHeight - 1) * 2.0 - 1.0;

                    vec3 dir = Normalize(vec3(u * frustumCornerVS.x, v * frustumCornerVS.y, -1.0));
                    Ray r(cameraPositionWS, dir);

                    color += GetRayColor(r, MaxBounces);
                }

                buffer[localIdx].r = sqrt(color.r * AAScale);
                buffer[localIdx].g = sqrt(color.g * AAScale);
                buffer[localIdx].b = sqrt(color.b * AAScale);
            }
        }

	}

    const Color CPURaytracer::GetRayColor(Ray& ray, const int depth) const
    {
        if (depth <= 0) return Color(0, 0, 0, 0);

        RayHitRecord rhr;
        double closestHitT = infinity;
        bool hasHit = false;

        for (const auto& obj : m_pRenderObjects)
        {
            RayHitRecord temp;
            if (obj->Hit(ray, 0.001, closestHitT, temp))
            {
                rhr = temp;
                closestHitT = temp.t;
                hasHit = true;
            }
        }

        if (hasHit)
        {
            // Do more bounces!
            // Bounces and attenuation color are determined by the material we just hit
            Color attenuationColor;
            Ray bounceRay;
            if (rhr.pMaterial->Scatter(ray, rhr, attenuationColor, bounceRay))
            {
                return attenuationColor * GetRayColor(bounceRay, depth - 1);
            }
            return Color(0, 0, 0, 0);
        }

        // Sky background
        vec3 unit_direction = Normalize(ray.GetDirection());
        auto t = 0.5 * (unit_direction.y + 1.0);
        auto c = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
        return Color(c.x, c.y, c.z, 0.f);
    }

    const bool CPURaytracer::HitSphere(const Ray& ray, vec3& hitPoint) const
    {
        // todo: move this
        vec3 spherePos = vec3(0, 0, 1);
        double rad = 0.5;

        // Test if in sphere
        vec3 displ = spherePos - ray.GetOrigin();
        double dSqr = Dot(displ, displ);
        bool inSphere = (dSqr <= rad * rad);

        double tPlane = Dot(displ, ray.GetDirection()); // projection of displ onto dir, note that |dir| == 1

        if (tPlane > 0)
        {
            // Find collision pt on plane and test against radius
            vec3 planeCollisionPt = ray.GetOrigin() + ray.GetDirection() * tPlane;
            vec3 collisionDispl = spherePos - planeCollisionPt;
            double cdSqr = Dot(collisionDispl, collisionDispl);

            // Alt version w/ pythagorean
            //cdSqr = dSqr - tPlane * tPlane;

            if (cdSqr <= rad * rad)
            {
                // Now move forward or backwards to find collision point
                double offset = std::sqrt(rad * rad - cdSqr);
                double tSurface = tPlane + (inSphere) ? offset : -offset;

                hitPoint = ray.GetOrigin() + ray.GetDirection() * tSurface;
                return true;
            }
        }
        return false;
    }

    const double CPURaytracer::hit_sphere(const vec3& center, double radius, const Ray& r) const
    {
        vec3 oc = r.GetOrigin() - center;
        auto a = Dot(r.GetDirection(), r.GetDirection());
        auto half_b = Dot(oc, r.GetDirection());
        auto c = Dot(oc, oc) - radius * radius;
        auto discriminant = half_b * half_b - a * c;

        if (std::abs(a) < 0.00001) a = 0.00001;

        // Return distance, or -1
        if (discriminant < 0.0) return -1.0;
        else return (-half_b - std::sqrt(discriminant)) / a;
    }
}