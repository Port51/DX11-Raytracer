#pragma once
#include "RayReceiver.h"
#include "AABB.h"
#include "RendererList.h"

namespace gfx
{
    class Ray;

    class BVHNode : public RayReceiver
    {
    public:
        BVHNode();
        BVHNode(RendererList& list);
        BVHNode(std::vector<std::shared_ptr<RayReceiver>>& src, const size_t start, const size_t end);
        
    public:
        virtual const bool Hit(const Ray& r, const double t_min, const double t_max, RayHitRecord& rec, const uint gBufferIdx) const override;
        virtual const bool GetAABB(AABB& aabb) const override;

    private:
        static bool CompareAABB_X(const std::shared_ptr<RayReceiver> a, const std::shared_ptr<RayReceiver> b);
        static bool CompareAABB_Y(const std::shared_ptr<RayReceiver> a, const std::shared_ptr<RayReceiver> b);
        static bool CompareAABB_Z(const std::shared_ptr<RayReceiver> a, const std::shared_ptr<RayReceiver> b);

    public:
        std::shared_ptr<RayReceiver> m_left;
        std::shared_ptr<RayReceiver> m_right;
        AABB m_aabb;
    };
}