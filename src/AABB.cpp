#include "AABB.h"

AABB::AABB() : x0(FLT_MAX),
               x1(-FLT_MAX),
               y0(FLT_MAX),
               y1(-FLT_MAX),
               z0(FLT_MAX),
               z1(-FLT_MAX) {}

AABB::~AABB() {}

void AABB::add(const QVector3D &point)
{
    x0 = std::min(x0, point.x());
    x1 = std::max(x1, point.x());
    y0 = std::min(y0, point.y());
    y1 = std::max(y1, point.y());
    z0 = std::min(z0, point.z());
    z1 = std::max(z1, point.z());
}

void AABB::combine(const AABB &aabb)
{
    x0 = std::min(x0, aabb.x0);
    x1 = std::max(x1, aabb.x1);
    y0 = std::min(y0, aabb.y0);
    y1 = std::max(y1, aabb.y1);
    z0 = std::min(z0, aabb.z0);
    z1 = std::max(z1, aabb.z1);
}

float AABB::rangeX() const
{
    return x1 - x0;
}

float AABB::rangeY() const
{
    return y1 - y0;
}

float AABB::rangeZ() const
{
    return z1 - z0;
}

bool AABB::trace(const Ray &ray) const
{
    QVector3D o = ray.getOrigin(), d = ray.getDirection();
    float t0 = 0.0f, t1 = FLT_MAX;
    // 射线若与AABB相交，则必然在每个轴上均与其相交，考虑单一轴进行推理：
    // 若将在该轴射线方向上将AABB的近极值点定义为x0，对应t值为t0，远极值点定义为x1，对应t值为t1，则t0<=t1，但t0与t1不一定大于0
    // 考虑到射线与AABB相交只可能发生在射线参数t>0的情况下：
    // 此时必然存在一点t0'射线进入AABB，存在另一点t1'射线离开AABB，其满足0=<t0'<=t1'<=t1和t0<=t0'，但t0是否大于0未可知
    // 若从反面判断，不满足上式，即t0'>t1'，等价于t0>t1，此时射线不可能与AABB在该轴上相交

    // 因此，利用上式子进行判断，一旦射线在某一轴不与AABB相交，立马可以断定射线不与AABB相交；

    // 若射线在某个轴上的分量为0，则视为与该轴平行，此时只需要考虑极值情况：
    // 即射线的原点是否位于AABB该坐标轴的范围之外
    // 否则进行上述判断
    if (std::fabs(d.x()) > EPSILON)
    {
        t0 = std::max(t0, ((d.x() > 0 ? x0 : x1) - o.x()) / d.x()); // t0>=0x'w
        t1 = std::min(t1, ((d.x() > 0 ? x1 : x0) - o.x()) / d.x());
    }
    else if (o.x() < x0 || o.x() > x1)
        return false;

    if (std::fabs(d.y()) > EPSILON)
    {
        t0 = std::max(t0, ((d.y() > 0 ? y0 : y1) - o.y()) / d.y());
        t1 = std::min(t1, ((d.y() > 0 ? y1 : y0) - o.y()) / d.y());
    }
    else if (o.y() < y0 || o.y() > y1)
        return false;

    if (std::fabs(d.z()) > EPSILON)
    {
        t0 = std::max(t0, ((d.z() > 0 ? z0 : z1) - o.z()) / d.z());
        t1 = std::min(t1, ((d.z() > 0 ? z1 : z0) - o.z()) / d.z());
    }
    else if (o.z() < z0 || o.z() > z1)
        return false;

    return t0 <= t1;
}