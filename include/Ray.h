#ifndef RAY_H
#define RAY_H

#include <QVector3D>
/**
 * @brief 光线类
 *
 */
class Ray
{
private:
    // 原点以及方向
    QVector3D origin, direction;

public:
    // 根据起点和方向构建射线
    Ray(const QVector3D &origin, const QVector3D &direction);
    ~Ray();
    QVector3D getOrigin() const;
    QVector3D getDirection() const;
    // 根据射线参数值获取对应点
    QVector3D point(const float t) const;
    // 根据法向量求入射光线镜面反射光线
    QVector3D reflect(const QVector3D &normal) const;
};

#endif