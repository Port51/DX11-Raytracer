#pragma once
#include "CommonHeader.h"

namespace gfx
{
	class Ray;

	class CPURaytracer
	{
	public:
		CPURaytracer();
		~CPURaytracer() = default;
	public:
		void RunTile(RGBA* const buffer, const uint tileX, const uint tileY) const;
	private:
		void GetRayColor(Ray& ray, RGBA* const bufferSegment) const;
		const bool HitSphere(const Ray& ray, vec3& hitPoint) const;
		const double hit_sphere(const vec3& center, double radius, const Ray& r) const;
	};
}