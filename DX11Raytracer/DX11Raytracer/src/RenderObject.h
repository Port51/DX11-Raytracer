#pragma once
#include "CommonHeader.h"

namespace gfx
{
	class Ray;

	struct RayHitRecord
	{
		vec3 p;
		vec3 normal;
		double t;
		bool isFrontFacing;
	};

	class RenderObject
	{
	public:
		RenderObject() {};
		virtual ~RenderObject() = default;
	public:
		virtual bool Hit(const Ray& ray, double t_min, double t_max, RayHitRecord& rec) const = 0;
	};
}