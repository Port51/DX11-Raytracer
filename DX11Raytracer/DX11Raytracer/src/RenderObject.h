#pragma once
#include "CommonHeader.h"

namespace gfx
{
	class Ray;
	class Material;

	class RenderObject
	{
	public:
		RenderObject() {};
		virtual ~RenderObject() = default;
	public:
		virtual bool Hit(const Ray& ray, const double t_min, const double t_max, RayHitRecord& rec) const = 0;
	};
}