#pragma once
#include "CommonHeader.h"
#include "RendererList.h"
#include "BVHNode.h"
#include <vector>

namespace gfx
{
	class Ray;
	class RayReceiver;
	class SphereObject;
	class Camera;
	class GBuffer;

	class CPURaytracer
	{
	public:
		CPURaytracer();
		~CPURaytracer() = default;
	public:
		void RunTile(const Camera& camera, Color* const buffer, const uint tileX, const uint tileY, const uint iteration, const GBuffer& gBuffer, const uint gBufferIdx) const;
	private:
		void CreateRandomScene();
		const Color GetRayColor(Ray& ray, const int depth, const GBuffer& gBuffer, const uint gBufferIdx) const;
		//const bool HitSphere(const Ray& ray, vec3& hitPoint) const;
	private:
		std::unique_ptr<BVHNode> m_pRendererList;
	};
}