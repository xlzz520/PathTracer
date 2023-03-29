#ifndef TEXTURE_H
#define TEXTURE_H

#include <QImage>
#include <QColor>
#include <QVector3D>
#include <QVector2D>

#include "UtilsHelper.h"
/**
 * @brief 纹理类
 *
 */

class Texture
{
private:
    QImage image;

public:
    Texture(const QImage &image);
    ~Texture();
    // 纹理是否为空
    bool isNull() const;
    // 根据uv坐标获取纹理
    QVector3D color(const QVector2D &uv) const;
};

#endif