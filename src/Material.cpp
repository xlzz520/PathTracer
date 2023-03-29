#include "Material.h"

Material::Material() {}

Material::Material(const QVector3D &diffuse, const QVector3D &specular, const QVector3D &emissive, const float shininess) : diffuse(diffuse),
                                                                                                                            specular(specular),
                                                                                                                            emissive(emissive),
                                                                                                                            shininess(shininess)
{
    // 没有镜面反射分量，此时只能按照漫反射pdf进行重要性采样生成光线
    if (specular.isNull())
        threshold = 1.0f + EPSILON;
    // 既有镜面反射也有漫反射，根据多重重要性采样原则，此时需要计算一个采样阈值来平衡两种采样方式
    else
    {
        float t = (shininess + 1.0f) * (1.0f - std::pow(0.5f, 1.0f / (shininess + 1.0f)));
        // 归一到[0,1]中
        threshold = t / (t + 1.0f);
    }
}

Material::~Material() {}

QVector3D Material::getEmissive() const
{
    return emissive;
}

float Material::getThreshold() const
{
    return threshold;
}

// 理想Lambertian漫反射BRDF
QVector3D Material::diffuseBRDF() const
{
    return diffuse / PI;
}

// 归一化系数为(shininess + 2.0f) / (PI * 2.0f)的Phong’s glossy reflection BRDF
QVector3D Material::specularBRDF(const QVector3D &reflection, const QVector3D &direction) const
{
    // 余弦夹角
    float cosine = std::max(QVector3D::dotProduct(reflection, direction), 0.0f);
    return specular * std::pow(cosine, shininess) * (shininess + 2.0f) / (PI * 2.0f);
}

// 根据材料的brdf进行多重重要性采样
// https://www.cs.princeton.edu/courses/archive/fall08/cos526/assign3/lawrence.pdf
// a physically plausible brdf based on Phong shading separates the reflectance distribution of a surface into a diffuse and specular component:
//   kd*1/pi + ks (n+2)/(2*pi)*cos(alpha)^n （kd+ks<=1 for the property of energy conservation）
void Material::sample(const QVector3D &normal, const QVector3D &reflection, const QVector3D &color, QVector3D &direction, QVector3D &albedo) const
{
    float theta, phi;
    QVector3D tangent, bitangent;
    // 为了生成出射光线，需要概率进行判断是根据漫反射还是高光反射射出光线
    if (randomUniform() <= threshold)
    {
        // 漫反射时theta为出射光线与法向量的夹角
        sampleHemisphere(1.0f, theta, phi);
        float cosine = std::cos(theta);
        float sine = std::sin(theta);
        calculateTangentSpace(normal, tangent, bitangent);
        direction = cosine * normal + sine * std::cos(phi) * tangent + sine * std::sin(phi) * bitangent;
        // albedo = diffuseBRDF/diffusePDF
        albedo = diffuse * color * std::max(QVector3D::dotProduct(direction, normal), 0.0f);
    }
    else
    {
        // 高光反射时theta为半程向量和法向量的夹角
        sampleHemisphere(shininess, theta, phi);
        float cosine = std::cos(theta);
        float sine = std::sin(theta);

        calculateTangentSpace(normal, tangent, bitangent);
        QVector3D half = cosine * normal + sine * std::cos(phi) * tangent + sine * std::sin(phi) * bitangent;

        // 半程向量转出射向量
        Ray tempray(QVector3D(0,0,0),-reflection);

        direction = 2 * half - tempray.reflect(half);

        // calculateTangentSpace(reflection, tangent, bitangent);
        // direction = cosine * reflection + sine * std::cos(phi) * tangent + sine * std::sin(phi) * bitangent;
        // if (QVector3D::dotProduct(normal, direction) < 0.0f)
        //     direction = cosine * reflection - sine * std::cos(phi) * tangent - sine * std::sin(phi) * bitangent;

        //  albedo = specularBRDF/specularPDF
        //         = specular * std::pow(cosine, shininess) * (shininess + 2.0f) / (PI * 2.0f)
        //             /( (shininess + 1) / ( PI * 2.0f ) * pow(cosine,shininess) )
        //         = specular * (shininess + 2)/(shininess + 1)
        // albedo = specularBRDF(reflection,direction)/specularPDF();

        albedo = specular * (shininess + 2) / (shininess + 1) * 4 * std::max(QVector3D::dotProduct(direction, half), 0.0f);
        albedo *= std::max(QVector3D::dotProduct(direction, normal), 0.0f);
    }
}