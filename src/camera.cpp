#include <cmath>

#include "tinyxml2/tinyxml2.h"

#include "camera.h"
#include "Ray.h"

Camera::Camera()
{
}

Camera::Camera(const tinyxml2::XMLDocument &xml)
{
    auto camera_xmlnode = xml.FirstChildElement("camera");

    float x, y, z;
    sscanf(camera_xmlnode->FirstChildElement("eye")->Attribute("x"), "%f", &x);
    sscanf(camera_xmlnode->FirstChildElement("eye")->Attribute("y"), "%f", &y);
    sscanf(camera_xmlnode->FirstChildElement("eye")->Attribute("z"), "%f", &z);
    eye = QVector3D(x, y, z);

    sscanf(camera_xmlnode->FirstChildElement("lookat")->Attribute("x"), "%f", &x);
    sscanf(camera_xmlnode->FirstChildElement("lookat")->Attribute("y"), "%f", &y);
    sscanf(camera_xmlnode->FirstChildElement("lookat")->Attribute("z"), "%f", &z);
    lookat = QVector3D(x, y, z);

    sscanf(camera_xmlnode->FirstChildElement("up")->Attribute("x"), "%f", &x);
    sscanf(camera_xmlnode->FirstChildElement("up")->Attribute("y"), "%f", &y);
    sscanf(camera_xmlnode->FirstChildElement("up")->Attribute("z"), "%f", &z);
    up = QVector3D(x, y, z);

    camera_xmlnode->QueryFloatAttribute("fovy", &(fovy));
    camera_xmlnode->QueryIntAttribute("width", &(width));
    camera_xmlnode->QueryIntAttribute("height", &(height));

    // forward
    QVector3D f = (lookat - eye).normalized();
    QVector3D u = up.normalized();
    QVector3D l = QVector3D::crossProduct(u, f);
    float scale = 2.0f * std::tan(fovy / 360.0f * PI) / (float)height;
    du = u * scale;
    dl = l * scale;

    // 左上角
    left_top_corner = eye + f + (du * (float)height + dl * (float)width) * 0.5f;
}

Ray Camera::cast_ray(int i, int j) const
{

    // x,y为层次采样扰动
    float x, y;
    samplePixel(x, y);
    // 求出方向
    QVector3D direction = (left_top_corner - dl * ((float)i + x) - du * ((float)j + y) - eye).normalized();

    return Ray(eye, direction);
}
int Camera::getWidth() const
{
    return width;
}
int Camera::getHeight() const
{
    return height;
}
