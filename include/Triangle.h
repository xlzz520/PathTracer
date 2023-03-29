#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <cfloat>
#include <cmath>

#include <QVector3D>
#include <QVector2D>

#include "ConfigHelper.h"
#include "UtilsHelper.h"
#include "Point.h"
#include "AABB.h"
#include "Ray.h"
/**
 * @brief 三角形类
 *
 */
class Triangle
{
private:
    // 三个顶点
    Point p0, p1, p2;
    // 重心
    QVector3D center;

public:
    Triangle(const Point &p0, const Point &p1, const Point &p2);
    ~Triangle();
    // 获取重心
    QVector3D getCenter() const;
    // 面积
    float area() const;
    // 获取AABB
    AABB aabb() const;
    // 三角形截交判断
    void trace(const Ray &ray, float &t, Point &point) const;
    // 从三角形中随机采样
    Point sample() const;
};

#endif