#pragma once
#include "CommonHeader.h"
#include <vector>

namespace gfx
{
	class Ray;
	class RenderObject;
	class SphereObject;
	class Camera;

	class CPURaytracer
	{
	public:
		CPURaytracer();
		~CPURaytracer() = default;
	public:
		void RunTile(const Camera& camera, Color* const buffer, const uint tileX, const uint tileY) const;
	private:
		void CreateRandomScene();
		void AddRenderObject(std::unique_ptr<RenderObject> pRenderObject);
		const Color GetRayColor(Ray& ray, const int depth) const;
		const bool HitSphere(const Ray& ray, vec3& hitPoint) const;
	private:
		std::vector<std::unique_ptr<RenderObject>> m_pRenderObjects;
	};
}