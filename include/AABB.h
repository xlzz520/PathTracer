#ifndef AABB_H
#define AABB_H

#include <cfloat>
#include <cmath>
#include <algorithm>

#include <QVector3D>

#include "ConfigHelper.h"
#include "Ray.h"

/**
 * @brief AABB数据结构
 */
class AABB
{
private:
    // 0为小值，1为大值
    float x0, x1, y0, y1, z0, z1;

public:
    AABB();
    ~AABB();
    // 增加点
    void add(const QVector3D &point);
    // 两个AABB融合
    void combine(const AABB &aabb);
    //返回X、Y、Z轴的范围
    float rangeX() const;
    float rangeY() const;
    float rangeZ() const;
    // 与光线进行相交判断
    bool trace(const Ray &ray) const;
};

#endif