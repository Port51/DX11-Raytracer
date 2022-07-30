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
#include "Skybox.h"
#include "BVHNode.h"

namespace gfx
{
	CPURaytracer::CPURaytracer()
	{
        switch (SceneSelection)
        {
            case 0u:
                CreateIceScene();
                break;
            case 1u:
                CreateSphereScene();
                break;
        }
    }

    CPURaytracer::~CPURaytracer()
    {}

    void CPURaytracer::CreateIceScene()
    {
        RendererList rendererList;

        auto material1 = std::make_shared<MetalMaterial>(Color(0.5f, 0.5f, 1.0f, 1.0f), 0.0);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0.25, 1.5, -2.45), 1.0, material1));

        auto material2 = std::make_shared<MetalMaterial>(Color(1.0f), 0.0);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0.25, 1.5, 4.05), 1.0, material2));

        auto material3 = std::make_shared<MetalMaterial>(Color(1.0f, 0.2f, 0.0f, 1.0f), 0.0);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0.25, 1.5, 0), 1.0, material3));

        auto material4 = std::make_shared<MetalMaterial>(Color(1.0f, 1.0f, 1.0f, 1.0f), 0.215);
        rendererList.Add(std::make_unique<SphereObject>(vec3(6.35, 0.475, -3.05), 0.427, material4));

        // Volumetric ice
        auto iceMaterial = std::make_shared<IceMaterial>();
        rendererList.Add(std::make_unique<IceSurface>(vec3(0, 0, 0), iceMaterial));

        m_pRendererList = std::make_unique<BVHNode>(rendererList);

        m_pSkybox = std::make_unique<Skybox>(
            Color(0.35f, 0.75f, 0.925f),
            Color(1.0f, 0.75f, 0.6f),
            Color(0.01f, 0.03f, 0.09f));
    }

    void CPURaytracer::CreateSphereScene()
    {
        RendererList rendererList;

        auto mirrorMaterial = std::make_shared<MetalMaterial>(Color(1.0f), 0.05);
        rendererList.Add(std::make_unique<SphereObject>(vec3(0.0, 1.5, 0.0), 1.5, mirrorMaterial));

        for (int a = -11; a < 11; a++)
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
        }

        auto groundMaterial = std::make_shared<LambertianMaterial>(std::make_shared<CheckeredTexture>(Color(0.5, 0.5, 0.5, 0.5), Color(0.25, 0.25, 1.0, 0.5)));
        auto grassMaterial = std::make_shared<LambertianMaterial>(Color(0.2, 0.8, 0.2, 1.0));
        rendererList.Add(std::make_unique<SphereObject>(vec3(0, -1000, 0), 1000, grassMaterial));

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
        if (depth <= 0) return Color(0.0);

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
        return m_pSkybox->GetColor(ray.GetDirection());
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