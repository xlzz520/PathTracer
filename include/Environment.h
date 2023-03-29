#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <cmath>

#include <QImage>
#include <QVector3D>

#include "ConfigHelper.h"
#include "Texture.h"
/**
 * @brief 环境光
 */
class Environment
{

private:
    Texture texture;

public:
    Environment();
    Environment(const Texture &texture);
    ~Environment();
    bool isNull() const;
    // 根据三维几何坐标获取对应的球形坐标作为纹理uv值（极点处会造成失真）
    QVector3D color(const QVector3D &direction) const;
};

#endif