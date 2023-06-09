#include "BVH.h"
// BVH的递归构建过程
// Top-down模式
BVH::BVH(const std::vector<Triangle> &triangles)
{
    // 当前节点的aabb位包络所有子节点的aabb
    for (const Triangle &triangle : triangles)
        aabb.combine(triangle.aabb());
    //小于BVH节点三角形分裂下限就不再进行分裂
    if (triangles.size() <= BVH_LIMIT)
    {
        this->triangles = triangles;
        left = right = nullptr;
    }
    else
    {
        // 选择最大轴物体数量中点进行划分
        float x = aabb.rangeX(), y = aabb.rangeY(), z = aabb.rangeZ();
        std::vector<Triangle> tempTriangles = triangles;
        if (x >= y && x >= z)
            std::sort(tempTriangles.begin(), tempTriangles.end(), compareByX);
        else if (y >= x && y >= z)
            std::sort(tempTriangles.begin(), tempTriangles.end(), compareByY);
        else
            std::sort(tempTriangles.begin(), tempTriangles.end(), compareByZ);
        // 划分BVH
        auto middle = tempTriangles.begin() + (tempTriangles.size() / 2);
        std::vector<Triangle> leftTriangles(tempTriangles.begin(), middle);
        std::vector<Triangle> rightTriangles(middle, tempTriangles.end());
        left = new BVH(leftTriangles);
        right = new BVH(rightTriangles);
    }
}

BVH::BVH(const BVH &bvh) : aabb(bvh.aabb),
                           triangles(bvh.triangles),
                           left(bvh.left == nullptr ? nullptr : new BVH(*bvh.left)),
                           right(bvh.right == nullptr ? nullptr : new BVH(*bvh.right)) {}

BVH::~BVH()
{
    delete left;
    delete right;
}

bool BVH::compareByX(const Triangle &t0, const Triangle &t1)
{
    return t0.getCenter().x() < t1.getCenter().x();
}

bool BVH::compareByY(const Triangle &t0, const Triangle &t1)
{
    return t0.getCenter().y() < t1.getCenter().y();
}

bool BVH::compareByZ(const Triangle &t0, const Triangle &t1)
{
    return t0.getCenter().z() < t1.getCenter().z();
}

// 光线与BVH截交计算
void BVH::trace(const Ray &ray, float &t, Point &point) const
{
    if (!aabb.trace(ray))
        t = FLT_MAX;
    else if (left != nullptr || right != nullptr)
    {
        float tLeft, tRight;
        Point pointLeft, pointRight;
        left->trace(ray, tLeft, pointLeft);
        right->trace(ray, tRight, pointRight);
        if (tLeft < tRight)
        {
            t = tLeft;
            point = pointLeft;
        }
        else
        {
            t = tRight;
            point = pointRight;
        }
    }
    else
    {
        t = FLT_MAX;
        for (const Triangle &triangle : triangles)
        {
            float tTemp;
            Point pointTemp;
            triangle.trace(ray, tTemp, pointTemp);
            if (tTemp < t)
            {
                t = tTemp;
                point = pointTemp;
            }
        }
    }
}