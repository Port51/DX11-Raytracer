#include "RendererList.h"

namespace gfx
{
    RendererList::RendererList() {}

    RendererList::~RendererList() {}

    RendererList::RendererList(std::shared_ptr<RayReceiver> object)
    {
        Add(object);
    }

    void RendererList::Clear()
    {
        m_renderers.clear();
    }

    std::vector<std::shared_ptr<RayReceiver>>& RendererList::GetRenderers()
    {
        return m_renderers;
    }

    const bool RendererList::Hit(const Ray& r, double t_min, double t_max, RayHitRecord& rec) const
    {
        RayHitRecord temp;
        auto hitAnything = false;
        auto closestSoFar = t_max;

        for (const auto& renderer : m_renderers)
        {
            if (renderer->Hit(r, t_min, closestSoFar, temp))
            {
                hitAnything = true;
                closestSoFar = temp.time;
                rec = temp;
            }
        }

        return hitAnything;
    }

    const bool RendererList::GetAABB(AABB& aabb) const
    {
        if (m_renderers.empty()) return false;

        AABB temp_box;
        bool first_box = true;

        for (const auto& object : m_renderers)
        {
            if (!object->GetAABB(temp_box)) return false;
            aabb = first_box ? temp_box : AABB::GetCombinedAABB(aabb, temp_box);
            first_box = false;
        }

        return true;
    }
}