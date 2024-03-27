#ifndef CAMERA_H
#define CAMERA_H
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QImage>
#include <QColor>
#include <omp.h>

#include <tinyxml2/tinyxml2.h>

#include "Ray.h"
#include "ConfigHelper.h"
#include "UtilsHelper.h"


/**
 * @brief 相机类
 * 
 */
class Camera
{
public:
    Camera();
    //根据xml文件初始化相机参数
    Camera(const tinyxml2::XMLDocument &xml);
    //根据像素坐标生成射线
    Ray cast_ray(int i, int j) const;
    int getWidth() const;
    int getHeight() const;
private:
    //相机参数
    QVector3D eye;
    QVector3D up;
    QVector3D lookat;
    float fovy;
    //图像宽度和高度
    int width, height;
    //图像平面参数
    QVector3D du, dl;
    QVector3D left_top_corner;
};

#endif