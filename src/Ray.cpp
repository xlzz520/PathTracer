#include "Ray.h"

Ray::Ray(const QVector3D &origin, const QVector3D &direction) : origin(origin),
                                                                direction(direction) {}

Ray::~Ray() {}

QVector3D Ray::getOrigin() const
{
    return origin;
}

QVector3D Ray::getDirection() const
{
    return direction;
}

QVector3D Ray::point(const float t) const
{
    return origin + t * direction;
}

QVector3D Ray::reflect(const QVector3D &normal) const
{
    return (direction - 2.0f * QVector3D::dotProduct(direction, normal) * normal).normalized();
}