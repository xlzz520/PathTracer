#ifndef MATERIAL_H
#define MATERIAL_H

#include <cmath>

#include <QVector3D>
#include "Ray.h"
#include "ConfigHelper.h"
#include "UtilsHelper.h"
/**
 * @brief 材质类，存储经典的phong shading需要的参数
 */
class Material
{
private:
    // 漫反射系数 镜面反射系数 自发射系数
    QVector3D diffuse, specular, emissive;
    // 透射率 折射系数(针对透明材料)
    QVector3D transmittance, ior;
    // 光滑度
    float shininess;
    // 采样阈值（判断是生成漫反射光线还是镜面反射光线）
    float threshold;

public:
    Material();
    Material(const QVector3D &diffuse, const QVector3D &specular, const QVector3D &emissive, const float shininess);
    ~Material();
    QVector3D getEmissive() const;
    float getThreshold() const;
    // 计算漫反射BRDF，view-independent
    QVector3D diffuseBRDF() const;
    // 计算镜面反射BRDF，view-dependent，输入为完美镜面反射和采样的反射光线
    QVector3D specularBRDF(const QVector3D &reflection, const QVector3D &direction) const;

    /**
     * @brief 根据材料的brdf进行重要性采样
     *
     * @param normal 输入的法线
     * @param reflection 输入的反射光线方向
     * @param color 输入的物体材质
     * @param direction 输出的采样生成出射方向
     * @param albedo 输出的brdf*cos/pdf
     */
    void sample(const QVector3D &normal, const QVector3D &reflection, const QVector3D &color, QVector3D &direction, QVector3D &albedo) const;
};

#endif