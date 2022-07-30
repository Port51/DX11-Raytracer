#pragma once
#include "RayReceiver.h"
#include "CommonHeader.h"
#include "Ray.h"
#include "AABB.h"
#include <memory>
#include <vector>

namespace gfx
{
    class RendererList : public RayReceiver
    {
    public:
        RendererList();
        ~RendererList();
        RendererList(std::shared_ptr<RayReceiver> object);

    public:
        void Clear();
        void Add(std::shared_ptr<RayReceiver> object) { m_renderers.push_back(object); }
        std::vector<std::shared_ptr<RayReceiver>>& GetRenderers();
        virtual const bool Hit(const Ray& r, const f32 t_min, const f32 t_max, RayHitRecord& rec, const uint gBufferIdx) const override;
        virtual const bool GetAABB(AABB& aabb) const override;

    private:
        std::vector<std::shared_ptr<RayReceiver>> m_renderers;
    };
}