#pragma once
#include "CommonHeader.h"
#include <vector>

namespace gfx
{
	class Ray;
	class RenderObject;
	class SphereObject;

	class CPURaytracer
	{
	public:
		CPURaytracer();
		~CPURaytracer() = default;
	public:
		void RunTile(Color* const buffer, const uint tileX, const uint tileY) const;
	private:
		const Color GetRayColor(Ray& ray, const int depth) const;
		const bool HitSphere(const Ray& ray, vec3& hitPoint) const;
		const double hit_sphere(const vec3& center, double radius, const Ray& r) const;

		std::vector<std::unique_ptr<RenderObject>> m_pRenderObjects;
	};
}