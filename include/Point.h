#ifndef POINT_H
#define POINT_H

#include <QVector3D>
#include <QVector2D>
/**
 * @brief 点，拥有位置和纹理信息
 * 
 */
class Point {
private:
    //位置和法向
    QVector3D position, normal;
    //纹理坐标
    QVector2D uv;

public:
    Point();
    Point(const QVector3D &position, const QVector3D &normal, const QVector2D &uv);
    ~Point();
    QVector3D getPosition() const;
    QVector3D getNormal() const;
    QVector2D getUV() const;
};

#endif