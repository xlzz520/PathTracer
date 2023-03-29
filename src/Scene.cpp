#include "Scene.h"

Scene::Scene() {}

Scene::Scene(const std::string &meshPath, const std::string &environmentPath)
{

    spdlog::set_level(spdlog::level::trace);
    spdlog::info("开始读取模型");
    double start = cpuSecond();

    // 利用Assimp读取场景obj文件，返回aiScene
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
    std::string directory = meshPath.substr(0, meshPath.find_last_of('/'));
    if (nullptr != scene)
        processNode(scene->mRootNode, scene, directory);
    // 读取环境贴图
    if (environmentPath.empty())
        environment = Environment(Texture(QImage()));
    else
        environment = Environment(Texture(QImage(environmentPath.c_str())));

    double end = cpuSecond();
    spdlog::info("读取完毕，共花费: {:.6f}s", end - start);
}

Scene::~Scene() {}

// aiScene是一个node-hierarchy
void Scene::processNode(const aiNode *node, const aiScene *scene, const std::string &directory)
{
    // 处理当前节点的mesh，节点存储的是索引，真正的mesh存储在aiMesh中
    for (int i = 0; i < node->mNumMeshes; i++)
        meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene, directory));
    // 递归处理子节点
    for (int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene, directory);
}

Mesh Scene::processMesh(const aiMesh *mesh, const aiScene *scene, const std::string &directory) const
{
    std::vector<Point> points;
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        QVector3D position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        QVector3D normal(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        QVector2D uv(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        if (mesh->mTextureCoords[0] != nullptr)
            uv = QVector2D(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        points.emplace_back(position, normal, uv);
    }
    std::vector<Triangle> triangles;
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        Point p0 = points[mesh->mFaces[i].mIndices[0]];
        Point p1 = points[mesh->mFaces[i].mIndices[1]];
        Point p2 = points[mesh->mFaces[i].mIndices[2]];
        triangles.emplace_back(p0, p1, p2);
    }
    // 一个mesh对应一个material
    aiMaterial *materialTemp = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D diffuseTemp, specularTemp, emissiveTemp;
    float shininess;
    materialTemp->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseTemp);
    materialTemp->Get(AI_MATKEY_COLOR_SPECULAR, specularTemp);
    materialTemp->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveTemp);
    materialTemp->Get(AI_MATKEY_SHININESS, shininess);
    QVector3D diffuse(diffuseTemp.r, diffuseTemp.g, diffuseTemp.b);
    QVector3D specular(specularTemp.r, specularTemp.g, specularTemp.b);
    QVector3D emissive(emissiveTemp.r, emissiveTemp.g, emissiveTemp.b);
    Material material(diffuse, specular, emissive, shininess);

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

QVector3D Scene::shade(const Ray &ray, const Point &point, const Material &material, const QVector3D &color, const int bounce) const
{
    QVector3D position = point.getPosition();
    QVector3D normal = point.getNormal();
    QVector3D reflection = ray.reflect(normal);

    // 根据表面渲染方程，output_color = self-emission_color + integeral_with_Li*brdf*cos_in_all_direction

    // 1.自发射光
    QVector3D ans = material.getEmissive();

    // 2.遍历光源，对光源进行采样（区域光），此时pdf为 1/area(A)
    //  也可以从光源中随机选择一个光源，然后再从光源中进行采样，此时pdf为 (area(A)/总的光源的面积)*1/area(A)
    //  但考虑到本次场景中光源数量均很少，效果差不多，因此选择第一种遍历光源形式
    for (const Mesh &mesh : meshes)
        if (!mesh.getMaterial().getEmissive().isNull())
        {

            QVector3D sum(0.0f, 0.0f, 0.0f);
            // 对此光源进行采样
            Point sample = mesh.sample();
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
                sum = mesh.getMaterial().getEmissive() * brdf * cosine0 * cosine1 * mesh.getArea() / (sample.getPosition() - position).lengthSquared();
            }

            ans += sum;
        }

    // 3.计算环境光
    if (!environment.isNull())
    {
        QVector3D sum(0.0f, 0.0f, 0.0f);
        QVector3D direction, albedo;
        // 根据brdf采样
        material.sample(normal, reflection, color, direction, albedo);

        Ray rayTemp(position, direction);
        float tTemp;
        Point pointTemp;
        Material materialTemp;
        QVector3D colorTemp;
        trace(rayTemp, tTemp, pointTemp, materialTemp, colorTemp);

        if (tTemp == FLT_MAX)
            sum = environment.color(direction) * albedo / (1 / (2 * PI));
        ans += sum;
    }

    // 4.计算间接照明
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
            //         Shade(p,-wi) * brdf * cosine  / pdf / p_rr
            ans += shade(rayTemp, pointTemp, materialTemp, colorTemp, bounce + 1) * albedo / (bounce < RUSSIAN_ROULETTE_THRESHOLD ? 1.0f : RUSSIAN_ROULETTE_PROBABILITY);
    }

    return ans;
}

void Scene::sample(const QVector3D &position, const QVector3D &o, const QVector3D &du, const QVector3D &dl, std::vector<std::vector<QVector3D>> &sum)
{
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("开始采样生成图像");
    double start = cpuSecond();

    // #pragma omp parallel for default(none) shared(position, o, du, dl, sum)

    // i,j为图像坐标
    for (int i = 0; i < sum.size(); i++)
        for (int j = 0; j < sum[i].size(); j++)
        {
            // x,y为层次采样扰动
            float x, y;
            samplePixel(x, y);
            // 求出方向
            QVector3D direction = (o - dl * ((float)i + x) - du * ((float)j + y) - position).normalized();

            Ray ray(position, direction);
            float t;
            Point point;
            Material material;
            QVector3D color;
            // 光追判断
            trace(ray, t, point, material, color);

            // 没有与场景中的物体截交
            if (t == FLT_MAX)
                sum[i][j] += environment.color(direction);
            // 与区域光源截交
            else if (!material.getEmissive().isNull())
                sum[i][j] += material.getEmissive();
            // 与物体截交
            else
                sum[i][j] += shade(ray, point, material, color, 0);

            std::cout << i << j << std::endl;
        }

    double end = cpuSecond();
    spdlog::info("采样生成图像完毕，共花费: {:.6f}s", end - start);
}