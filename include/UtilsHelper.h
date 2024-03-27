#ifndef UTILS_HELPER_H
#define UTILS_HELPER_H

#include <random>
#include <ctime>
#include <cmath>

#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <QVector3D>
#include <QColor>

#include "ConfigHelper.h"

// 随机数引擎
static std::default_random_engine engine(std::time(nullptr));
// 随机数分布
static std::uniform_real_distribution<float> uniform(0.0f, 1.0f);

static QVector3D colorToVector(const QColor &color)
{
    float x = (float)color.red() / 255.0f;
    float y = (float)color.green() / 255.0f;
    float z = (float)color.blue() / 255.0f;

    return QVector3D(x, y, z);
}

// 将[0,1]之间的x,y,z值进行伽马矫正后转化为[0,255]的R、G和B值
static QColor vectorToColor(const QVector3D &vector)
{
    int r = std::min((int)(std::pow(vector.x(), 1.0f / GAMMA) * 255), 255);
    int g = std::min((int)(std::pow(vector.y(), 1.0f / GAMMA) * 255), 255);
    int b = std::min((int)(std::pow(vector.z(), 1.0f / GAMMA) * 255), 255);
    
    return QColor(r, g, b);
}

// 根据normal，构造tangent和bitangent，形成一组正交基
static void calculateTangentSpace(const QVector3D &normal, QVector3D &tangent, QVector3D &bitangent)
{
    tangent = std::fabs(normal.x()) < EPSILON && std::fabs(normal.y()) < EPSILON ? QVector3D(1.0f, 0.0f, 0.0f) : QVector3D(normal.y(), -normal.x(), 0.0f).normalized();
    bitangent = QVector3D::crossProduct(tangent, normal);
}

//[0,1]均匀分布随机采样
static float randomUniform()
{
    return uniform(engine);
}

// 使用分层采样来对每个像素点进行采样
static void samplePixel(float &x, float &y)
{
    int xt = (int)(randomUniform() * STRATIFY_SIZE);
    int yt = (int)(randomUniform() * STRATIFY_SIZE);
    x = ((float)xt + randomUniform()) / STRATIFY_SIZE;
    y = ((float)yt + randomUniform()) / STRATIFY_SIZE;
}
// 半球积分采样
// 对于均匀采样：
//  pdf(w)=1/(2*pi)
//  pdf(theta,phi)=sin(theta)/(2*pi)
//  pdf(theta)=sin(theta) -> cdf(theta)=1-cos(theta)
//  pdf(phi)=1/(2*pi)     -> cdf( phi )=phi/(2*pi)
//  最后逆采样即可
// 对于考虑余弦的漫反射
//  pdf=1/pi * cos theta
//  最终得到theta = acos(√random)
// 对于余弦加权镜面反射
//  pdf=(n+1)/(2*pi)*cos(theta)^n
// 最终得到theta=acos(random^(1/n))
static void sampleHemisphere(const float exp, float &theta, float &phi)
{

    theta = std::acos(std::pow(randomUniform(), 1.0f / (exp + 1.0f)));
    phi = randomUniform() * PI * 2.0f;
}

// 获取cpu时间
static double cpuSecond(void)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    //                秒                 微秒
    return ((double)tp.tv_sec + (double)tp.tv_usec * 1e-6);
}

#endif