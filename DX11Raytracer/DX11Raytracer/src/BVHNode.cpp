#include "BVHNode.h"

namespace gfx
{

    BVHNode::BVHNode()
    {}

    BVHNode::BVHNode(RendererList & list)
        : BVHNode(list.GetRenderers(), 0, list.GetRenderers().size())
    {}

    BVHNode::BVHNode(std::vector<std::shared_ptr<RayReceiver>>& src, const size_t start, const size_t end)
    {
        const size_t receiverCt = end - start;
        if (receiverCt == 1)
        {
            m_left = m_right = src[start];
        }
        else if (receiverCt == 2)
        {
            m_left = src[start];
            m_right = src[start + 1];
        }
        else
        {
            // Find extents of centers, which will be used as an estimate for where to create a split
            vec3f minPosition = src.at(start)->GetCurrentPosition();
            vec3f maxPosition = minPosition;
            for (size_t i = 1, ct = src.size(); i < ct; ++i)
            {
                minPosition = MinVec3(minPosition, src.at(i)->GetCurrentPosition());
                maxPosition = MinVec3(maxPosition, src.at(i)->GetCurrentPosition());
            }
            const vec3f range = maxPosition - minPosition;

            // Choose split most likely to be evenly divided
            const auto comparator = (range.x > range.y && range.x > range.z) ? CompareAABB_X
                : (range.y > range.z) ? CompareAABB_Y
                : CompareAABB_Z;

            std::sort(src.begin() + start, src.begin() + end, comparator);

            const size_t midPt = start + receiverCt / 2;
            m_left = std::make_shared<BVHNode>(src, start, midPt);
            m_right = std::make_shared<BVHNode>(src, midPt, end);
        }

        AABB box_left, box_right;

        if (!m_left->GetAABB(box_left) || !m_right->GetAABB(box_right))
        {
            std::cerr << "No bounding box in bvh_node constructor.\n";
        }

        m_aabb = AABB::GetCombinedAABB(box_left, box_right);
    }

    const bool BVHNode::GetAABB(AABB& aabb) const
    {
        aabb = m_aabb;
        return true;
    }

    const bool BVHNode::Hit(const Ray& r, const f32 t_min, const f32 t_max, RayHitRecord& rec, const uint gBufferIdx) const
    {
        if (!m_aabb.Hit(r, t_min, t_max))
            return false;

        const bool hit_left = m_left->Hit(r, t_min, t_max, rec, gBufferIdx);
        const bool hit_right = m_right->Hit(r, t_min, hit_left ? rec.time : t_max, rec, gBufferIdx);

        return hit_left || hit_right;
    }

    bool BVHNode::CompareAABB_X(const std::shared_ptr<RayReceiver> a, const std::shared_ptr<RayReceiver> b)
    {
        AABB aabb0, aabb1;
        if (!a->GetAABB(aabb0) || !b->GetAABB(aabb1)) std::cerr << "No bounding box in bvh_node constructor.\n";
        return aabb0.GetMinimum().x < aabb1.GetMinimum().x;
    }

    bool BVHNode::CompareAABB_Y(const std::shared_ptr<RayReceiver> a, const std::shared_ptr<RayReceiver> b)
    {
        AABB aabb0, aabb1;
        if (!a->GetAABB(aabb0) || !b->GetAABB(aabb1)) std::cerr << "No bounding box in bvh_node constructor.\n";
        return aabb0.GetMinimum().y < aabb1.GetMinimum().y;
    }

    bool BVHNode::CompareAABB_Z(const std::shared_ptr<RayReceiver> a, const std::shared_ptr<RayReceiver> b)
    {
        AABB aabb0, aabb1;
        if (!a->GetAABB(aabb0) || !b->GetAABB(aabb1)) std::cerr << "No bounding box in bvh_node constructor.\n";
        return aabb0.GetMinimum().z < aabb1.GetMinimum().z;
    }
}