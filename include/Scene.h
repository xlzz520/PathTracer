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
#include <QString>
#include <QVector3D>
#include <QVector2D>
#include <QImage>
#include <QColor>
#include <omp.h>

#include "ConfigHelper.h"
#include "UtilsHelper.h"
#include "Point.h"
#include "Triangle.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Environment.h"
#include "Ray.h"

#include <spdlog/spdlog.h>

/**
 * @brief 场景类
 */
class Scene
{
private:
    // 物体网格序列
    std::vector<Mesh> meshes;
    // 环境贴图
    Environment environment;
    
    //利用assimp 读取obj文件和mtl文件时的处理函数
    void processNode(const aiNode *node, const aiScene *scene, const std::string &directory);
    Mesh processMesh(const aiMesh *mesh, const aiScene *scene, const std::string &directory) const;
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
    Scene(const std::string &meshPath, const std::string &environmentPath = "");
    ~Scene();
    
    /**********************************************************************************************/
    /**
     * @brief 场景采样函数，对每个像素点进行场景采样后形成一幅图像
     * 
     * @param position 输入的机的原点位置
     * @param o 输入的成像平面最值位置
     * @param du 输入的单位像素高对应在成像平面的位置变化值
     * @param dl 输入的单位像素宽对应在成像平面的位置变化值
     * @param sum 输出的结果图像
     */
    void sample(const QVector3D &position, const QVector3D &o, const QVector3D &du, const QVector3D &dl, std::vector<std::vector<QVector3D>> &sum);
};

#endif



// 柴老师，在听取了子理学长的意见后，我一定要和你聊聊：
// 1. 我真的从来没有想过偷懒，我每天作息基本上10左右起来后，就会开始工作，一直到晚上10点（以前去实验室的时候我顺便吃完饭11左右到实验室，然后后面只外出吃一趟晚饭），
// 不管有没有在实验室我都在工作，因为我本科就是一直卷过来的，我自己也想读博拿研究生的国奖，也知道必须保证足够的时间科研。我觉得可能的问题是我和你沟通少了，让你对我有了一些成见。这是我自己的原因，因为我觉得我
// 得做成一些大成果后才能和你细聊（比如实验取得突破性进展、哪里需要申请批示这些的），我后面会坚持每周给你一个进度汇报，不管我进度有没有很大。
//    我自己的社交活动真的很少了，这学期我就去了今天一次，我参加了计算机学院研究生会，今天下午得举办一个面向全学院的就业分享会，这是我的一些学生工作。

// 2. 我自己真的很怕被说闲话，比如担心拿了钱不干活啥的，因为真的我如果全身心科研我现在能力不够不一定会及时给你带来什么论文产出，所以老师你可以给我安排
// 一些横向，我真的一点意见都没有，其实我上次就已经和你沟通过了（我身边很多同学发顶会都是1年左右才会有产出，纯科研真的产出不稳定，我自己需要很多时间积累如果老师你信任我的话）；
// 另一方面我在和魏浩然他们聊的时候，他们也问我为什么不参与这种包装网页的设计，因为我是计算机背景的入手起来这种前端后端代码都很快，他们不是的。老师你不发工资给我都行，我
// 都没有任何意见，我真的怕别人说我偷懒不干活。

// 3. 关于太火鸟这边我的一些对接情况，我是和子理请示后子理说可以请假我就和田总说了（我这周天天都在赶一个作业，计算机图形学渲染器真的要写很久我自己课业都忙不过来）。
// 我没有想到他会反应这么大，说什么耽误他们商业进度啥的。我是被突然拉进这个太火鸟技术组的我之前也不知道一个咋情况，只是得每周完成一些他们需要实现的一些调研。 

//    一些真实情况必须要提到的是：我和子理都不是很看好他们的一些需求（太难了，很多根本不是我们几个研一的小硕士做不出来，而是和微软合作他们都不一定能满足他们那个商业需求），
// 他们自己也觉得可能做不出来这种，我没想到会突然扯到商业进度（他们把一个这么重要的任务交给我太太太信任我了），他们一直在提出不断的新的需求，上周要我去解决一个阿里都解决不
// 了的一个3d变形。。
//     除了让我做3d生成，现在让我做图像分割，我都在上周五调研之后跟他们说了一些方案，说下周开始跑实验，我也没有答应他们说我下周一定能搞出来，因为我觉得任务难度真的很大
//     葛志超学长也和我说过他和太火鸟的合作经历，基本上一学期都是在调研没有做出任何东西来。
//     太火鸟是一个眼特别高的公司，子理让我的做事态度是尽可能用一些事实说服他们不能做，可以做的话最好。

// 然后我前面说的总结就是：
// 1. 我没有偷懒，我真的很忙，不是什么每天花天酒地去这去那到处社交的，但我要保证尽可能每周和老师你沟通一下进度。
// 2. 我可以接横向，我现在只是个研一的小硕士，我自己科研能力不稳定，一直怕没有论文产出被说闲话。
// 3. 太火鸟这边的一些对我来说很难的”调研需求“我尽力满足，我这周末尽量给他们一个汇报，别耽误他们的进度。 