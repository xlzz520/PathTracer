#ifndef BVH_H
#define BVH_H

#include <cfloat>
#include <algorithm>
#include <vector>

#include "ConfigHelper.h"
#include "Point.h"
#include "Triangle.h"
#include "AABB.h"
#include "Ray.h"
/**
 * @brief 层次包围体结构，用于加速光线与场景截交计算
 *
 */

class BVH
{
private:
    // 当前节点的aabb
    AABB aabb;
    // 当前节点的三角形序列
    std::vector<Triangle> triangles;
    // 左右子节点分支
    BVH *left, *right;
    static bool compareByX(const Triangle &t0, const Triangle &t1);
    static bool compareByY(const Triangle &t0, const Triangle &t1);
    static bool compareByZ(const Triangle &t0, const Triangle &t1);

public:
    // 从三角形列表中递归地构建出BVH
    BVH(const std::vector<Triangle> &triangles);
    BVH(const BVH &bvh);
    ~BVH();
    // 射线与BVH截交计算，返回对应的t值和相交点
    void trace(const Ray &ray, float &t, Point &point) const;
};

#endif