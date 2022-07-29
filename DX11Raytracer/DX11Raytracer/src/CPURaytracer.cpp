#include "CPURaytracer.h"
#include "Settings.h"
#include "Ray.h"
#include "RayReceiver.h"
#include "SphereObject.h"
#include "IceSurface.h"
#include "LambertianMaterial.h"
#include "MetalMaterial.h"
#include "DielectricMaterial.h"
#include "IceMaterial.h"
#include "Camera.h"
#include "CheckeredTexture.h"
#include "GBuffer.h"

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
        //rendererList.Add(std::make_unique<SphereObject>(vec3(0, -1000, 0), 1000, grassMaterial));

        /*for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                double choice = Random::RandomDouble();
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
        }*/

        //auto material1 = std::make_shared<DielectricMaterial>(1.5);
        auto material1 = std::make_shared<MetalMaterial>(Color(0.5f, 0.5f, 1.0f, 1.0f), 0.0);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0.95, 1.5, -2.05), 1.0, material1));

        //auto material2 = std::make_shared<LambertianMaterial>(Color(0.4f, 0.2f, 0.1f, 1.0f));
        auto material2 = std::make_shared<MetalMaterial>(Color(1.0f), 0.0);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0.95, 1.5, 2.05), 1.0, material2));

        auto material3 = std::make_shared<MetalMaterial>(Color(1.0f, 0.2f, 0.0f, 1.0f), 0.0);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0.95, 1.5, 0), 1.0, material3));

        //auto lambertMaterial = std::make_shared<LambertianMaterial>(Color(1.0f, 0.3f, 0.1f, 1.0f));
        //rendererList.Add(std::make_unique<SphereObject>(vec3(-8.0, 0.55, 13.5), 0.42, lambertMaterial));
        //rendererList.Add(std::make_unique<SphereObject>(vec3(-7.5, 0.55, 14.5), 0.42, lambertMaterial));
        //rendererList.Add(std::make_unique<SphereObject>(vec3(-7.0, 0.55, 15.5), 0.42, lambertMaterial));

        // Volumetric ice
        auto iceMaterial = std::make_shared<IceMaterial>();
        rendererList.Add(std::make_unique<IceSurface>(vec3(0, 0, 0), iceMaterial));

        m_pRendererList = std::make_unique<BVHNode>(rendererList);
    }

	void CPURaytracer::RunTile(const Camera& camera, Color* const buffer, const uint tileX, const uint tileY, const uint passIteration, const GBuffer& gBuffer, const uint gBufferIdx) const
	{
		const int tileOffset = (tileY * TileDimensionX + tileX) * (TileSize * TileSize);

        const int maxBounces = 20;
        const int samplesPerPixel = (gBufferIdx == 0u) ? 5 : 1;
        const float sampleScale = 1.0f / samplesPerPixel;
        const bool useDepthOfField = (gBufferIdx == 0u) ? UseDepthOfField : false;

        const float multisampleScale0 = static_cast<float>(passIteration);
        const float multisampleScale1 = 1.0f / static_cast<float>(passIteration + 1u);

        for (uint lx = 0u; lx < TileSize; ++lx)
        {
            for (uint ly = 0u; ly < TileSize; ++ly)
            {
                const int x = TileSize * tileX + lx;
                const int y = TileSize * tileY + ly;
                const int pixelIdx = y * ScreenWidth + x;

                Color pixelColor;
                for (int a = 0; a < samplesPerPixel; ++a)
                {
                    // NDC coords
                    // Randomness creates AA, but shouldn't be used for ice volumetrics
                    const double sx = static_cast<double>(x) + ((gBufferIdx != 1u) ? Random::RandomDouble(-0.5, 0.5) : 0.0);
                    const double sy = static_cast<double>(y) + ((gBufferIdx != 1u) ? Random::RandomDouble(-0.5, 0.5) : 0.0);
                    const double u = sx / (ScreenWidth - 1) * 2.0 - 1.0;
                    const double v = sy / (ScreenHeight - 1) * 2.0 - 1.0;

                    Ray r = camera.GetRay(u, v, pixelIdx, useDepthOfField);
                    if (pixelIdx == 512 * 128 + 256)
                    {
                        auto vv = 0;
                    }
                    pixelColor += (GetRayColor(r, maxBounces, gBuffer, gBufferIdx, passIteration) * sampleScale);
                }

                if (gBufferIdx == 1u)
                {
                    // Find max within sample ranges
                    if (!UseRaymarchSlices)
                    {
                        // New, accurate method
                        const float alphaMult = 1.f;// Saturate(1.f - buffer[pixelIdx].a);
                        buffer[pixelIdx].r += pixelColor.r * alphaMult;
                        buffer[pixelIdx].g += pixelColor.g * alphaMult;
                        buffer[pixelIdx].b += pixelColor.b * alphaMult;
                        buffer[pixelIdx].a = pixelColor.a;

                        // Nice debug view for seeing individual slices
                        //buffer[pixelIdx] = pixelColor;
                    }
                    else
                    {
                        // Old "comb" method
                        buffer[pixelIdx].r = max(buffer[pixelIdx].r, pixelColor.r);
                        buffer[pixelIdx].g = max(buffer[pixelIdx].g, pixelColor.g);
                        buffer[pixelIdx].b = max(buffer[pixelIdx].b, pixelColor.b);
                    }
                }
                else
                {
                    // Average over time (with min depth stored in alpha)
                    buffer[pixelIdx].r = (buffer[pixelIdx].r * multisampleScale0 + pixelColor.r) * multisampleScale1;
                    buffer[pixelIdx].g = (buffer[pixelIdx].g * multisampleScale0 + pixelColor.g) * multisampleScale1;
                    buffer[pixelIdx].b = (buffer[pixelIdx].b * multisampleScale0 + pixelColor.b) * multisampleScale1;
                    buffer[pixelIdx].a = min(buffer[pixelIdx].a, pixelColor.a);
                }
                
            }
        }

	}

    const Color CPURaytracer::GetRayColor(Ray& ray, const int depth, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const
    {
        if (depth <= 0.0) return Color(0.0);

        RayHitRecord rhr;
        if (m_pRendererList->Hit(ray, 0.001, Infinity, rhr, gBufferIdx))
        {
            // Do more bounces!
            // Bounces and attenuation color are determined by the material we just hit
            Color attenuationColor;
            Color emittedColor;
            Ray bounceRay;
            if (rhr.pMaterial->Scatter(ray, rhr, attenuationColor, emittedColor, bounceRay, gBuffer, gBufferIdx, passIteration))
            {
                return emittedColor + attenuationColor * GetRayColor(bounceRay, depth - 1, gBuffer, gBufferIdx, passIteration);
            }
            return emittedColor;
        }

        // Sky background
        const vec3 upColor = vec3(0.2, 0.7, 0.95);
        const vec3 horizonColor = vec3(1.0, 0.75, 0.6);
        const vec3 deepColor = vec3(0.01, 0.03, 0.09);

        const vec3 rayDirNorm = Normalize(ray.GetDirection());
        const double vertical = std::abs(rayDirNorm.y);
        const double skyLerp = std::pow(vertical, 0.25);
        const vec3 skyColor = (1.0 - skyLerp) * horizonColor + skyLerp * upColor;

        const double deepWaterLerp = std::pow(vertical, 0.09);
        const vec3 deepWaterColor = (1.0 - deepWaterLerp) * horizonColor + deepWaterLerp * deepColor;

        const vec3 backgroundColor = (rayDirNorm.y > 0.0) ? skyColor : deepWaterColor;

        return Color((float)backgroundColor.x, (float)backgroundColor.y, (float)backgroundColor.z, 0.f);
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