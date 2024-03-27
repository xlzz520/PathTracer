#include "Scene.h"

Scene::Scene() {}

Scene::Scene(const std::string &meshPath, bool threshold_method)
{

    spdlog::set_level(spdlog::level::trace);
    spdlog::info("开始读取模型");
    double start = cpuSecond();

    this->threshold_method=threshold_method;
    // 利用Assimp读取场景obj文件，返回aiScene
    Assimp::Importer importer; // 后处理：强制为三角形、翻转纹理
    const aiScene *scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
    std::string directory = meshPath.substr(0, meshPath.find_last_of('/'));
    std::string xmlpath = meshPath.substr(0, meshPath.find_last_of('.')) + ".xml";

    tinyxml2::XMLDocument doc;
    doc.LoadFile(xmlpath.c_str());
    auto lightnode = doc.FirstChildElement("light");
    std::map<std::string, QVector3D> lightmap;
    while (lightnode != nullptr)
    {
        float x, y, z;
        sscanf(lightnode->Attribute("radiance"), "%f,%f,%f", &x, &y, &z);
        QVector3D curLight(x, y, z);
        lightmap[lightnode->Attribute("mtlname")] = curLight;
        lightnode = lightnode->NextSiblingElement("light");
    }

    if (scene)
        processNode(scene->mRootNode, scene, directory, lightmap);
    else
    {
        spdlog::critical("模型读取失败！");
        return;
    }

    double end = cpuSecond();
    spdlog::info("模型读取完毕，共花费: {:.6f}s", end - start);
}

Scene::~Scene() {}

// aiScene是一个node-hierarchy，进行递归处理
void Scene::processNode(const aiNode *node, const aiScene *scene, const std::string &directory, std::map<std::string, QVector3D> lightmap)
{
    // 处理当前节点的mesh，节点存储的是索引，真正的mesh存储在aiMesh中
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *aimesh = scene->mMeshes[node->mMeshes[i]];
        Mesh mesh = processMesh(aimesh, scene, directory, lightmap);
        meshes.push_back(mesh);
        if (!mesh.getMaterial().getEmissive().isNull())
        {
            light_meshes.emplace_back(mesh);
            lightarea += mesh.getArea();
        }
    }
    // 递归处理子节点
    for (int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene, directory, lightmap);
}

Mesh Scene::processMesh(const aiMesh *mesh, const aiScene *scene, const std::string &directory, std::map<std::string, QVector3D> lightmap) const
{

    // 处理顶点
    std::vector<Point> points;
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        // 处理顶点位置、法线和纹理坐标
        QVector3D position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        QVector3D normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        QVector2D uv(0, 0);
        if (mesh->mTextureCoords[0] != nullptr)
            uv = QVector2D(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        points.emplace_back(position, normal, uv);
    }
    // 处理三角形
    std::vector<Triangle> triangles;
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        Point p0 = points[mesh->mFaces[i].mIndices[0]];
        Point p1 = points[mesh->mFaces[i].mIndices[1]];
        Point p2 = points[mesh->mFaces[i].mIndices[2]];
        triangles.emplace_back(p0, p1, p2);
    }

    // 处理材质 一个mesh对应一个material
    aiMaterial *materialTemp = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D diffuseTemp, specularTemp, transmittanceTemp;
    float shininess, ior;
    materialTemp->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseTemp);
    materialTemp->Get(AI_MATKEY_COLOR_SPECULAR, specularTemp);

    // 从xml文件中查询是否有对应材料名字的light
    aiString materialName;
    materialTemp->Get(AI_MATKEY_NAME, materialName);
    std::string lightname = materialName.C_Str();
    QVector3D emissive;
    if (lightmap.count(lightname))
    {
        emissive = lightmap[lightname];
    }
    materialTemp->Get(AI_MATKEY_COLOR_TRANSPARENT, transmittanceTemp);
    materialTemp->Get(AI_MATKEY_SHININESS, shininess);
    materialTemp->Get(AI_MATKEY_REFRACTI, ior);
    QVector3D diffuse(diffuseTemp.r, diffuseTemp.g, diffuseTemp.b);
    QVector3D specular(specularTemp.r, specularTemp.g, specularTemp.b);
    QVector3D transmittance(transmittanceTemp.r, transmittanceTemp.g, transmittanceTemp.b);
    Material material(diffuse, specular, emissive, shininess, transmittance, ior, threshold_method);

    // 处理diffuse纹理
    Texture texture = processTexture(materialTemp, directory);

    return Mesh(triangles, material, texture);
}

Texture Scene::processTexture(const aiMaterial *material, const std::string &directory) const
{
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        aiString nameTemp;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &nameTemp);
        std::string name = nameTemp.C_Str();
        return Texture(QImage((directory + "/" + name).c_str()));
    }
    else
        return Texture(QImage());
}

// 根据ray遍历每个mesh进行光线追踪，返回截交点t、位置、材料和纹理等相关信息
void Scene::trace(const Ray &ray, float &t, Point &point, Material &material, QVector3D &color) const
{
    t = FLT_MAX;
    for (const Mesh &mesh : meshes)
    {
        float tTemp;
        Point pointTemp;
        mesh.trace(ray, tTemp, pointTemp);
        if (tTemp < t)
        {
            t = tTemp;
            point = pointTemp;
            material = mesh.getMaterial();
            color = mesh.color(point.getUV());
        }
    }
}

// 在读取材料的自发射系数后，根据材料是玻璃材料还是Phong材料进行不同的积分渲染处理
QVector3D Scene::shade(const Ray &ray, const Point &point, const Material &material, const QVector3D &color, const int bounce) const
{
    QVector3D position = point.getPosition();
    QVector3D normal = point.getNormal();
    QVector3D reflection = ray.reflect(normal);

    QVector3D sum(0.0f, 0.0f, 0.0f);
    // 自发射光          +    积分处理
    // 根据表面渲染方程，output_color = self-emission_color + integeral_with_Li*brdf*cos_in_all_direction

    // 1.自发射光
    QVector3D ans = material.getEmissive();

    // 积分处理
    /************************************************************************/

    // 2.玻璃材料
    //   进行折射计算判断（模拟理想的折射btdf）
    if (material.getIor() > 1 && (bounce < RUSSIAN_ROULETTE_THRESHOLD || randomUniform() < RUSSIAN_ROULETTE_PROBABILITY))
    {
        QVector3D direction, albedo;
        // 玻璃材料的采样
        material.refract(normal, ray, direction, albedo);

        Ray rayTemp(position, direction);
        float tTemp;
        Point pointTemp;
        Material materialTemp;
        QVector3D colorTemp;
        trace(rayTemp, tTemp, pointTemp, materialTemp, colorTemp);
        if (tTemp < FLT_MAX)
        {
            ans += shade(rayTemp, pointTemp, materialTemp, colorTemp, bounce + 1) * albedo / (bounce < RUSSIAN_ROULETTE_THRESHOLD ? 1.0f : RUSSIAN_ROULETTE_PROBABILITY);
        }
        return ans;
    }

    // Phong材料
    //  3.遍历光源，对光源进行采样（区域光），
    //    可以从光源中随机选择一个光源，然后再从光源中进行采样，此时pdf为 (area(A)/总的光源的面积)*1/area(A)
    //    也可以遍历所有的光源，实验中选取这种方案，效果更好能加快收敛, 此时pdf为 1/area(A)

    for (Mesh mesh : light_meshes)
    {

        // 对此光源进行采样
        Point sample = mesh.sample(point);
        QVector3D direction = (sample.getPosition() - position).normalized();
        Ray rayTemp(position, direction);
        float tTemp;
        Point pointTemp;
        Material materialTemp;
        QVector3D colorTemp;
        trace(rayTemp, tTemp, pointTemp, materialTemp, colorTemp);

        // 能直接看到光源，没有被遮挡
        if ((pointTemp.getPosition() - sample.getPosition()).lengthSquared() < EPSILON)
        {
            float cosine0 = std::max(QVector3D::dotProduct(normal, direction), 0.0f);
            float cosine1 = std::max(QVector3D::dotProduct(sample.getNormal(), -direction), 0.0f);
            QVector3D brdf;
            // 计算着色点的brdf
            if (randomUniform() <= material.getThreshold())
                brdf = material.diffuseBRDF() * color;
            else
                brdf = material.specularBRDF(reflection, direction);
            // 入射光 * brdf * cosine0 * cosine1 / pdf_light / squared_distance
            sum += mesh.getMaterial().getEmissive() * brdf * cosine0 * cosine1 * mesh.getArea() / (sample.getPosition() - position).lengthSquared();
        }
    }
    ans += sum;
    
    // 4.计算Phong材料的间接照明
    if (bounce < RUSSIAN_ROULETTE_THRESHOLD || randomUniform() < RUSSIAN_ROULETTE_PROBABILITY)
    {
        QVector3D direction, albedo;
        // 根据brdf采样
        material.sample(normal, reflection, color, direction, albedo);

        Ray rayTemp(position, direction);
        float tTemp;
        Point pointTemp;
        Material materialTemp;
        QVector3D colorTemp;
        trace(rayTemp, tTemp, pointTemp, materialTemp, colorTemp);

        if (tTemp < FLT_MAX && materialTemp.getEmissive().isNull())
        {
            //         Shade(p,-wi) * brdf * cosine  / pdf / p_rr
            ans += shade(rayTemp, pointTemp, materialTemp, colorTemp, bounce + 1) * albedo / (bounce < RUSSIAN_ROULETTE_THRESHOLD ? 1.0f : RUSSIAN_ROULETTE_PROBABILITY);
        }
    }
    return ans;
}

void Scene::sample(const Camera &cam, std::vector<std::vector<QVector3D>> &sum)
{

    double start = cpuSecond();

#pragma omp parallel for schedule(dynamic)
    // i,j为图像坐标
    for (int i = 0; i < sum.size(); i++)
        for (int j = 0; j < sum[i].size(); j++)
        {
            // 形成射线
            Ray ray = cam.cast_ray(i, j);
            float t;
            Point point;
            Material material;
            QVector3D color;
            // 光追判断
            trace(ray, t, point, material, color);
            // 没有与场景中的物体截交
            if (t == FLT_MAX)
                sum[i][j] += QVector3D(0, 0, 0);
            // 与区域光源截交
            else if (!material.getEmissive().isNull())
                sum[i][j] += material.getEmissive();
            // 与物体截交
            else
                sum[i][j] += shade(ray, point, material, color, 0);
        }
}