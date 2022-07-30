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

        auto material1 = std::make_shared<MetalMaterial>(Color(0.5f, 0.5f, 1.0f, 1.0f), 0.0f);
        rendererList.Add(std::make_unique<SphereObject>(vec3f(0.25f, 1.5f, -2.45f), 1.0f, material1));

        auto material2 = std::make_shared<MetalMaterial>(Color(1.0f), 0.0f);
        rendererList.Add(std::make_unique<SphereObject>(vec3f(0.25f, 1.5f, 4.05f), 1.0f, material2));

        auto material3 = std::make_shared<MetalMaterial>(Color(1.0f, 0.2f, 0.0f, 1.0f), 0.0f);
        rendererList.Add(std::make_unique<SphereObject>(vec3f(0.25f, 1.5f, 0.f), 1.0f, material3));

        auto material4 = std::make_shared<MetalMaterial>(Color(1.0f, 1.0f, 1.0f, 1.0f), 0.215f);
        rendererList.Add(std::make_unique<SphereObject>(vec3f(6.35f, 0.475f, -3.05f), 0.427f, material4));

        // Volumetric ice
        auto iceMaterial = std::make_shared<IceMaterial>();
        rendererList.Add(std::make_unique<IceSurface>(vec3f(0.f, 0.f, 0.f), iceMaterial));

        m_pRendererList = std::make_unique<BVHNode>(rendererList);

        m_pSkybox = std::make_unique<Skybox>(
            Color(0.35f, 0.75f, 0.925f),
            Color(0.9f, 0.775f, 1.0f),
            Color(0.01f, 0.03f, 0.09f));
    }

    void CPURaytracer::CreateSphereScene()
    {
        RendererList rendererList;

        auto mirrorMaterial = std::make_shared<MetalMaterial>(Color(1.0f), 0.05f);
        rendererList.Add(std::make_unique<SphereObject>(vec3f(0.0f, 1.5f, 0.0f), 1.5f, mirrorMaterial));

        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                f32 choice = Random::RandomFloat();
                vec3f center(a + 0.9f * Random::RandomFloat(), 0.2f, b + 0.9f * Random::RandomFloat());

                if ((center - vec3f(4.f, 0.2f, 0.f)).Length() > 0.9f)
                {
                    if (choice < 0.8)
                    {
                        // diffuse
                        auto albedo = Color::Random() * Color::Random();
                        auto material = std::make_shared<LambertianMaterial>(albedo);
                        rendererList.Add(std::make_unique<SphereObject>(center, 0.2f, material));
                    }
                    else if (choice < 0.95)
                    {
                        // metal
                        auto albedo = Color::Random(0.5f, 1.f);
                        auto fuzz = Random::RandomFloat(0.f, 0.5f);
                        auto material = std::make_shared<MetalMaterial>(albedo, fuzz);
                        rendererList.Add(std::make_unique<SphereObject>(center, 0.2f, material));
                    }
                    else
                    {
                        // glass
                        auto material = std::make_shared<DielectricMaterial>(1.5f);
                        rendererList.Add(std::make_unique<SphereObject>(center, 0.2f, material));
                    }
                }
            }
        }

        auto groundMaterial = std::make_shared<LambertianMaterial>(std::make_shared<CheckeredTexture>(Color(0.5f, 0.5f, 0.5f, 0.5f), Color(0.25f, 0.25f, 1.0f, 0.5f)));
        auto grassMaterial = std::make_shared<LambertianMaterial>(Color(0.2f, 0.8f, 0.2f, 1.0f));
        rendererList.Add(std::make_unique<SphereObject>(vec3f(0.f, -1000.f, 0.f), 1000.f, grassMaterial));

        m_pRendererList = std::make_unique<BVHNode>(rendererList);
    }

	void CPURaytracer::RunTile(const Camera& camera, Color* const buffer, const uint tileX, const uint tileY, const uint passIteration, const GBuffer& gBuffer, const uint gBufferIdx) const
	{
		const int tileOffset = (tileY * TileDimensionX + tileX) * (TileSize * TileSize);

        const int maxBounces = 20;
        const int samplesPerPixel = (gBufferIdx == 0u) ? 5 : 1;
        const f32 sampleScale = 1.0f / samplesPerPixel;
        const bool useDepthOfField = (gBufferIdx == 0u) ? UseDepthOfField : false;

        const f32 multisampleScale0 = static_cast<f32>(passIteration);
        const f32 multisampleScale1 = 1.0f / static_cast<f32>(passIteration + 1u);

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
                    const f32 sx = static_cast<f32>(x) + ((gBufferIdx != 1u) ? Random::RandomFloat(-0.5f, 0.5f) : 0.0f);
                    const f32 sy = static_cast<f32>(y) + ((gBufferIdx != 1u) ? Random::RandomFloat(-0.5f, 0.5f) : 0.0f);
                    const f32 u = sx / (ScreenWidth - 1) * 2.0f - 1.0f;
                    const f32 v = sy / (ScreenHeight - 1) * 2.0f - 1.0f;

                    Ray r = camera.GetRay(u, v, pixelIdx, useDepthOfField);
                    pixelColor += (GetRayColor(r, maxBounces, gBuffer, gBufferIdx, passIteration) * sampleScale);
                }

                if (gBufferIdx == 1u)
                {
                    // Find max within sample ranges
                    if (!UseRaymarchSlices)
                    {
                        // New, accurate method
                        const f32 alphaMult = 1.f;// Saturate(1.f - buffer[pixelIdx].a);
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
                        buffer[pixelIdx].r = std::max(buffer[pixelIdx].r, pixelColor.r);
                        buffer[pixelIdx].g = std::max(buffer[pixelIdx].g, pixelColor.g);
                        buffer[pixelIdx].b = std::max(buffer[pixelIdx].b, pixelColor.b);
                    }
                }
                else
                {
                    // Average over time (with min depth stored in alpha)
                    buffer[pixelIdx].r = (buffer[pixelIdx].r * multisampleScale0 + pixelColor.r) * multisampleScale1;
                    buffer[pixelIdx].g = (buffer[pixelIdx].g * multisampleScale0 + pixelColor.g) * multisampleScale1;
                    buffer[pixelIdx].b = (buffer[pixelIdx].b * multisampleScale0 + pixelColor.b) * multisampleScale1;
                    buffer[pixelIdx].a = std::min(buffer[pixelIdx].a, pixelColor.a);
                }
                
            }
        }

	}

    const Color CPURaytracer::GetRayColor(Ray& ray, const int depth, const GBuffer& gBuffer, const uint gBufferIdx, const uint passIteration) const
    {
        if (depth <= 0.f) return Color(0.0f);

        RayHitRecord rhr;
        if (m_pRendererList->Hit(ray, 0.001f, Infinity, rhr, gBufferIdx))
        {
            // Calculate fog factor
            const f32 fogFactor = (gBufferIdx == 0u) ? Saturate(1.f - std::exp(rhr.time * rhr.time * -0.000177f)) : 0.f;

            // Do more bounces!
            // Bounces and attenuation color are determined by the material we just hit
            Color attenuationColor;
            Color emittedColor;
            Ray bounceRay;
            if (rhr.pMaterial->Scatter(ray, rhr, attenuationColor, emittedColor, bounceRay, gBuffer, gBufferIdx, passIteration))
            {
                // Apply fog to result
                return Lerp(emittedColor + attenuationColor * GetRayColor(bounceRay, depth - 1, gBuffer, gBufferIdx, passIteration), m_pSkybox->GetFogColor(), fogFactor);
            }

            // Apply fog to result
            return Lerp(emittedColor, m_pSkybox->GetFogColor(), fogFactor);
        }

        // Hit sky
        return (gBufferIdx == 0u) ? m_pSkybox->GetColor(ray.GetDirection()) : Color(0.f);
    }
}