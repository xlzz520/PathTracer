#ifndef MESH_H
#define MESH_H

#include <vector>

#include "UtilsHelper.h"
#include "Point.h"
#include "Triangle.h"
#include "BVH.h"
#include "Material.h"
#include "Texture.h"
#include "Ray.h"
/**
 * @brief 网格类，一个网格类对应一个物体（及其BVH），一种材料，一种可能的纹理
 * 
 */
class Mesh
{
private:
    //三角形序列
    std::vector<Triangle> triangles;
    //三角形序列中每个三角形对应的面积
    std::vector<float> areas;
    //总的面积
    float area;
    //网格对应的BVH
    BVH bvh;
    //材料
    Material material;
    //纹理
    Texture texture;

public:
    Mesh(const std::vector<Triangle> &triangles, const Material &material, const Texture &texture);
    ~Mesh();
    float getArea() const;
    Material getMaterial() const;
    //根据uv纹理坐标返回对应的纹理
    QVector3D color(const QVector2D &uv) const;
    //光线与物体网格进行截交计算
    void trace(const Ray &ray, float &t, Point &point) const;
    //对物体进行采样（用于光源采样）
    Point sample() const;
};

#endif