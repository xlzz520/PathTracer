#ifndef SCENE_H
#define SCENE_H

#include <cfloat>
#include <cmath>
#include <string>
#include <algorithm>
#include <vector>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <tinyxml2/tinyxml2.h>

#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QImage>
#include <QImageReader>
#include <QColor>
#include <omp.h>

#include "ConfigHelper.h"
#include "UtilsHelper.h"
#include "Point.h"
#include "Triangle.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Ray.h"
#include "camera.h"
#include <spdlog/spdlog.h>

/**
 * @brief 场景类
 */
class Scene
{
private:
    // 物体网格序列
    std::vector<Mesh> meshes;
    // 光源网格集合
    std::vector<Mesh> light_meshes;
    // 光源面积
    float lightarea;
    // 阈值方法，默认为true，即平等法
    bool threshold_method;
    //利用assimp 读取obj文件和mtl文件时的处理函数
    void processNode(const aiNode *node, const aiScene *scene, const std::string &directory,std::map<std::string,QVector3D> lightmap);
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene, const std::string &directory,std::map<std::string,QVector3D> lightmap) const;
    Texture processTexture(const aiMaterial *material, const std::string &directory) const;

    /**********************************************************************************************/
    /**
     * @brief 光追函数，计算光线与场景中物体网格可能的交点
     *
     * @param ray 输入光线
     * @param t 输出交点对应光线的参数t
     * @param point 输出交点的位置
     * @param material 输出交点对应的网格材料
     * @param color 输出交点对应的物体纹理
     */
    void trace(const Ray &ray, float &t, Point &point, Material &material, QVector3D &color) const;
    
    /**********************************************************************************************/
    /**
     * @brief  着色函数，在光线的截交点进行着色绘制
     *
     * @param ray 输入光线
     * @param point 输入着色点
     * @param material 输入材料
     * @param color 输入颜色
     * @param bounce 输入光线弹射次数
     * @return QVector3D 输出的最终渲染的颜色
     */
    QVector3D shade(const Ray &ray, const Point &point, const Material &material, const QVector3D &color, const int bounce) const;
    

public:
    Scene();
    Scene(const std::string &meshPath, bool threshold_method);
    ~Scene();
    
    /**********************************************************************************************/
    /**
     * @brief 场景采样函数，对每个像素点进行场景采样后形成一幅图像
     * 
     * @param cam 输入的相机模型
     * @param sum 输出的结果图像
     */
    void sample(const Camera &cam, std::vector<std::vector<QVector3D>> &sum);

};

#endif
