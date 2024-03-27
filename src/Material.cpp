#include "Material.h"

Material::Material() {}

Material::Material(const QVector3D &diffuse, const QVector3D &specular, const QVector3D &emissive, const float shininess, const QVector3D transmittance, const float ior, bool threshold_method) : diffuse(diffuse),
                                                                                                                                                                                                   specular(specular),
                                                                                                                                                                                                   emissive(emissive),
                                                                                                                                                                                                   shininess(shininess),
                                                                                                                                                                                                   transmittance(transmittance),
                                                                                                                                                                                                   ior(ior),
                                                                                                                                                                                                   threshold_method(threshold_method)
{

    // 没有镜面反射分量，此时只能按照漫反射pdf进行重要性采样生成光线
    if (specular.isNull())
        threshold = 1.0f + EPSILON;
    // 既有镜面反射也有漫反射，此时需要计算一个采样阈值来平衡两种采样方式
    else
    {
        if (threshold_method)
        {
            // 方法1：限制高光法
            float t = (shininess + 1.0f) * (1.0f - std::pow(0.5f, 1.0f / (shininess + 1.0f)));
            threshold = t / (t + 1.0f);
        }
        else // 方法2：平等法
        {
            float maxdiffuse = qMax(qMax(diffuse.x(), diffuse.y()), diffuse.z());
            float maxspecular = qMax(qMax(specular.x(), specular.y()), specular.z());
            float t = maxdiffuse / (maxdiffuse + maxspecular);
        }
    }
}

Material::~Material() {}

QVector3D Material::getEmissive() const
{
    return emissive;
}
float Material::getIor() const
{
    return ior;
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

// 根据材料的brdf进行重要性采样
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
        // 考虑cos 的 重要性采样
        // 漫反射时theta为出射光线与法向量的夹角
        sampleHemisphere(1.0f, theta, phi);
        float cosine = std::cos(theta);
        float sine = std::sin(theta);
        calculateTangentSpace(normal, tangent, bitangent);
        direction = cosine * normal + sine * std::cos(phi) * tangent + sine * std::sin(phi) * bitangent;
        // albedo = diffuseBRDF/diffusePDF
        albedo = diffuse * color;
    }
    else
    {
        // 考虑cos^n 的重要性采样
        //  高光反射时theta为半程向量和法向量的夹角
        sampleHemisphere(shininess, theta, phi);
        float cosine = std::cos(theta);
        float sine = std::sin(theta);

        calculateTangentSpace(reflection, tangent, bitangent);

        direction = cosine * reflection + sine * std::cos(phi) * tangent + sine * std::sin(phi) * bitangent;

        // // 半程向量转出射向量
        // Ray tempray(QVector3D(0, 0, 0), -reflection);

        // direction = 2 * half - tempray.reflect(half);

        if (QVector3D::dotProduct(normal, direction) < 0.0f)
            direction = cosine * reflection - sine * std::cos(phi) * tangent - sine * std::sin(phi) * bitangent;

        albedo = specular * (shininess + 2) / (shininess + 1) * std::max(QVector3D::dotProduct(direction, normal), 0.0f);
    }
}

// 计算菲涅尔系数
float schlick(float cosine, float ref_idx)
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::powf((1 - cosine), 5);
}
// 能不能折射
bool Can_Refract(const QVector3D &v, const QVector3D &n, float ni_over_nt, QVector3D &refracted)
{
    QVector3D uv = v.normalized();
    float dt = QVector3D::dotProduct(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
    if (discriminant > 0)
    {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else
        return false;
}
// 玻璃材料的出射光线采样计算
void Material::refract(const QVector3D &normal, const Ray &ray, QVector3D &direction, QVector3D &albedo) const
{
    QVector3D incidence = ray.getDirection();

    float curior;
    QVector3D curnormal;
    float cosine;
    // 玻璃 -> 空气
    if (QVector3D::dotProduct(normal, incidence) > EPSILON)
    {
        curior = ior;
        curnormal = -normal;
        cosine = ior * QVector3D::dotProduct(normal, incidence) / incidence.length();
    }
    else // 空气 -> 玻璃
    {
        curior = 1.0f / ior;
        curnormal = normal;
        cosine = -QVector3D::dotProduct(normal, incidence) / incidence.length();
    }
    //  全反射判断
    bool can_refract = Can_Refract(incidence, curnormal, curior, direction);
    float reflect_prob;
    if (can_refract)
    {
        reflect_prob = schlick(cosine, ior);
    }
    else
    {
        reflect_prob = 1;
    }
    // 菲尼尔现象
    if (randomUniform() < reflect_prob)
    {
        direction = ray.reflect(normal);
        albedo = QVector3D(1, 1, 1);
    }
    else
    {
        albedo = transmittance;
    }
}
