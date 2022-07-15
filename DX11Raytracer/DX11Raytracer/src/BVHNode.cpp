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
        auto objects = src; // Create a copy

        const int axis = Random::RandomInt(0, 2);
        auto comparator = (axis == 0) ? CompareAABB_X
            : (axis == 1) ? CompareAABB_Y
            : CompareAABB_Z;

        const size_t receiverCt = end - start;

        if (receiverCt == 1)
        {
            m_left = m_right = objects[start];
        }
        else if (receiverCt == 2)
        {
            if (comparator(objects[start], objects[start + 1]))
            {
                m_left = objects[start];
                m_right = objects[start + 1];
            }
            else
            {
                m_left = objects[start + 1];
                m_right = objects[start];
            }
        }
        else
        {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto midPt = start + receiverCt / 2;
            m_left = std::make_shared<BVHNode>(objects, start, midPt);
            m_right = std::make_shared<BVHNode>(objects, midPt, end);
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

    const bool BVHNode::Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec) const
    {
        if (!m_aabb.Hit(r, t_min, t_max))
            return false;

        bool hit_left = m_left->Hit(r, t_min, t_max, rec);
        bool hit_right = m_right->Hit(r, t_min, hit_left ? rec.t : t_max, rec);

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