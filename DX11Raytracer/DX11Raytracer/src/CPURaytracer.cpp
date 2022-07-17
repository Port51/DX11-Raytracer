#include "CPURaytracer.h"
#include "Ray.h"
#include "RayReceiver.h"
#include "SphereObject.h"
#include "LambertianMaterial.h"
#include "MetalMaterial.h"
#include "DielectricMaterial.h"
#include "Camera.h"
#include "CheckeredTexture.h"

namespace gfx
{
	CPURaytracer::CPURaytracer()
	{
        CreateRandomScene();

        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(0, 0, -1), 0.5, std::make_shared<Lambertian>(Color(1.0, 0.4, 0.4, 1.0))));
        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(1, 0, -1), 0.5, std::make_shared<Metal>(Color(1.0, 0.4, 0.4, 1.0), 0.0)));
        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(-1, 0, -1), 0.5, std::make_shared<Metal>(Color(1.0, 1.0, 1.0, 1.0), 0.5)));
        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(-1, 0, -1), -0.5, std::make_shared<Dielectric>(1.5)));
        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(-0.35, 0, -0.5), 0.27, std::make_shared<Dielectric>(1.5)));

        // Ground
        //m_pRenderObjects.emplace_back(std::make_unique<SphereObject>(vec3(0, -100.5, -1), 100, std::make_shared<Lambertian>(Color(0.1, 0.8, 0.2, 1.0))));
    }

    void CPURaytracer::CreateRandomScene()
    {
        RendererList rendererList;

        /*auto groundMaterial0 = std::make_shared<LambertianMaterial>(std::make_shared<CheckeredTexture>(Color(0.5, 0.5, 0.5, 0.5), Color(0.25, 0.25, 1.0, 0.5)));
        rendererList.Add(std::make_unique<SphereObject>(vec3(0, 0, 0), 1.0, groundMaterial0));
        rendererList.Add(std::make_unique<SphereObject>(vec3(0, -1001, 0), 1000, groundMaterial0));
        m_pRendererList = std::make_unique<BVHNode>(rendererList);
        return;*/

        auto groundMaterial = std::make_shared<LambertianMaterial>(std::make_shared<CheckeredTexture>(Color(0.5, 0.5, 0.5, 0.5), Color(0.25, 0.25, 1.0, 0.5)));
        auto grassMaterial = std::make_shared<LambertianMaterial>(Color(0.2, 0.8, 0.2, 1.0));
        rendererList.Add(std::make_unique<SphereObject>(vec3(0, -1000, 0), 1000, grassMaterial));

        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                auto choice = Random::RandomDouble();
                vec3 center(a + 0.9 * Random::RandomDouble(), 0.2, b + 0.9 * Random::RandomDouble());

                if ((center - vec3(4, 0.2, 0)).Length() > 0.9)
                {
                    if (choice < 0.8)
                    {
                        // diffuse
                        auto albedo = Color::Random() * Color::Random();
                        auto material = std::make_shared<LambertianMaterial>(albedo);
                        rendererList.Add(std::make_unique<SphereObject>(center, 0.2, material));
                    }
                    else if (choice < 0.95)
                    {
                        // metal
                        auto albedo = Color::Random(0.5, 1);
                        auto fuzz = Random::RandomDouble(0, 0.5);
                        auto material = std::make_shared<MetalMaterial>(albedo, fuzz);
                        rendererList.Add(std::make_unique<SphereObject>(center, 0.2, material));
                    }
                    else
                    {
                        // glass
                        auto material = std::make_shared<DielectricMaterial>(1.5);
                        rendererList.Add(std::make_unique<SphereObject>(center, 0.2, material));
                    }
                }
            }
        }

        auto material1 = std::make_shared<DielectricMaterial>(1.5);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0, 1, 0), 1.0, material1));

        auto material2 = std::make_shared<LambertianMaterial>(Color(0.4f, 0.2f, 0.1f, 1.0f));
        rendererList.Add(std::make_unique<SphereObject>(vec3(-4, 1, 0), 1.0, material2));

        auto material3 = std::make_shared<MetalMaterial>(Color(0.7f, 0.6f, 0.5f, 1.0f), 0.0);
        rendererList.Add(std::make_unique<SphereObject>(vec3(4, 1, 0), 1.0, material3));

        m_pRendererList = std::make_unique<BVHNode>(rendererList);
    }

	void CPURaytracer::RunTile(const Camera& camera, Color* const buffer, const uint tileX, const uint tileY) const
	{
		const int tileOffset = (tileY * TileDimensionX + tileX) * (TileSize * TileSize);

        const int maxBounces = 20;
        const int samplesPerPixel = 51;
        const float multisampleScale = 1.f / samplesPerPixel;

        for (int lx = 0; lx < TileSize; ++lx)
        {
            for (int ly = 0; ly < TileSize; ++ly)
            {
                const int x = TileSize * tileX + lx;
                const int y = TileSize * tileY + ly;
                const int localIdx = ly * TileSize + lx + tileOffset;

                Color color;
                for (int a = 0; a < samplesPerPixel; ++a)
                {
                    // NDC coords
                    // Randomness creates AA
                    const double u = static_cast<double>(x + Random::RandomDouble(-0.5, 0.5)) / (ScreenWidth - 1) * 2.0 - 1.0;
                    const double v = static_cast<double>(y + Random::RandomDouble(-0.5, 0.5)) / (ScreenHeight - 1) * 2.0 - 1.0;

                    Ray r = camera.GetRay(u, v);
                    color += GetRayColor(r, maxBounces);
                }

                buffer[localIdx].r = sqrt(color.r * multisampleScale);
                buffer[localIdx].g = sqrt(color.g * multisampleScale);
                buffer[localIdx].b = sqrt(color.b * multisampleScale);
            }
        }

	}

    const Color CPURaytracer::GetRayColor(Ray& ray, const int depth) const
    {
        if (depth <= 0) return Color(0, 0, 0, 0);

        RayHitRecord rhr;
        if (m_pRendererList->Hit(ray, 0.001, Infinity, rhr))
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
        const vec3 rayDirNorm = Normalize(ray.GetDirection());
        const auto vertical = rayDirNorm.y * 0.5 + 0.5;
        const auto skyColor = (1.0 - vertical) * vec3(1.0, 1.0, 1.0) + vertical * vec3(0.5, 0.7, 1.0);
        return Color((float)skyColor.x, (float)skyColor.y, (float)skyColor.z, 0.f);
    }

    /*const bool CPURaytracer::HitSphere(const Ray& ray, vec3& hitPoint) const
    {
        // todo: move this
        vec3 spherePos = vec3(0, 0, 1);
        double rad = 0.5;

        // Test if in SphereObject
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
    }*/
}