#include "ScenePrimitive.h"
#include "lib/BGRA.h"
#include "shapes/CS123Primitive.h"
#include "shapes/CS123Cone.h"
#include "shapes/CS123Cube.h"
#include "shapes/CS123Cylinder.h"
#include "shapes/CS123Mesh.h"
#include "shapes/CS123Sphere.h"
#include "shapes/CS123Torus.h"
#include "gl/textures/TextureParametersBuilder.h"
//#include "gl/shapes/CS123Primitive.h"
#include "gl/textures/Texture2D.h"

#include <QGLWidget>
#include <iostream>

using namespace CS123::GL;

struct textureCacheNode {
    textureCacheNode(const QImage &i) : image(i), texture2D_ptr(nullptr) {
        if (i.format() != QImage::Format_RGB32) {
            image = i.convertToFormat(QImage::Format_RGB32);
        }

        QImage fImage = QGLWidget::convertToGLFormat(image);
        texture2D_ptr = std::make_unique<Texture2D>(fImage.bits(), fImage.width(), fImage.height());

        TextureParametersBuilder builder;
        builder.setFilter(TextureParameters::FILTER_METHOD::LINEAR);
        builder.setWrap(TextureParameters::WRAP_METHOD::REPEAT);
        TextureParameters parameters = builder.build();
        parameters.applyTo(*texture2D_ptr);
    }

    QImage image;
    std::unique_ptr<Texture2D> texture2D_ptr;
};

std::map<std::string, textureCacheNode> textureCache;

ScenePrimitive::ScenePrimitive(const ScenePrimitiveData &scenePrimitive)
: m_p1(-1), m_p2(-1), m_p3(-1)
, m_modelTransform(1.0f)
, m_data(scenePrimitive)
, m_textureFileName()
{
    auto &material = m_data.material;
    if (material.textureMap.isUsed) {
        if (textureCache.find(material.textureMap.filename) == textureCache.end()) {
            // try to load image into cache
            QImage image(material.textureMap.filename.data());

            if (image.isNull()) {
                std::cerr << "failed open image: " << material.textureMap.filename << std::endl;
                return;
            }

            std::cout << "loaded image: " << material.textureMap.filename << std::endl;

            textureCache.insert(std::make_pair(material.textureMap.filename, image));
        }
        m_textureFileName = material.textureMap.filename;
    }
}

ScenePrimitive::~ScenePrimitive()
{
}

bool ScenePrimitive::textureUsed() const {
    return !m_textureFileName.empty();
}

Texture2D &ScenePrimitive::getTexture2D() const {
    return *textureCache.at(m_textureFileName).texture2D_ptr;
}

BGRA ScenePrimitive::sampleTexture(const glm::vec2 &uv, const glm::vec2 &repeatUV) const {
    QImage &image = textureCache.at(m_textureFileName).image;

    int w = image.width(), h = image.height();
    float u = uv.x, v = 1.0f - uv.y; // use same uv as OpenGL (origin at bottom left)
    float j = repeatUV.x, k = repeatUV.y;

    int s = static_cast<int>(u * w * j) % w;
    int t = static_cast<int>(v * h * k) % h;

    return reinterpret_cast<BGRA*>(image.bits())[t * w + s];
}

ScenePrimitive& ScenePrimitive::update(int p1, int p2, float p3) {
    switch (this->type()) {
        case PrimitiveType::PRIMITIVE_CONE:
            if (p1 != m_p1 || p2 != m_p2)
                m_primitive_ptr = std::make_unique<CS123Cone>(p1, p2);
            break;

        case PrimitiveType::PRIMITIVE_CUBE:
            if (p1 != m_p1)
                m_primitive_ptr = std::make_unique<CS123Cube>(p1);
            break;

        case PrimitiveType::PRIMITIVE_CYLINDER:
            if (p1 != m_p1 || p2 != m_p2)
                m_primitive_ptr = std::make_unique<CS123Cylinder>(p1, p2);
            break;

        case PrimitiveType::PRIMITIVE_TORUS:
            if (p1 != m_p1 || p2 != m_p2 || p3 != m_p3)
                m_primitive_ptr = std::make_unique<CS123Torus>(p1, p2, p3);
            break;

        case PrimitiveType::PRIMITIVE_SPHERE:
            if (p1 != m_p1 || p2 != m_p2)
                m_primitive_ptr = std::make_unique<CS123Sphere>(p1, p2);
            break;

        case PrimitiveType::PRIMITIVE_MESH:
            // TODO: mesh?
            m_primitive_ptr = std::make_unique<CS123Mesh>();
    }

    m_p1 = p1; m_p2 = p2; m_p3 = p3;

    return *this;
}

ScenePrimitive& ScenePrimitive::draw() {
    if (m_primitive_ptr)
        m_primitive_ptr->draw();
    return *this;
}

bool ScenePrimitive::castRay(const glm::vec4 &startPoint, const glm::vec4 &worldSpaceRay,
                             float &min_t, glm::vec4 &normal, glm::vec2 &uv) const {
    bool hit = false;
    // to object space
    glm::vec4 p = glm::inverse(m_modelTransform) * startPoint;
    glm::vec4 ray = glm::inverse(m_modelTransform) * worldSpaceRay;

    switch (this->type()) {
        case PrimitiveType::PRIMITIVE_CONE:
            hit = CS123Cone::castRay(p, ray, min_t, normal, uv);
            break;

        case PrimitiveType::PRIMITIVE_CUBE:
            hit = CS123Cube::castRay(p, ray, min_t, normal, uv);
            break;

        case PrimitiveType::PRIMITIVE_CYLINDER:
            hit = CS123Cylinder::castRay(p, ray, min_t, normal, uv);
            break;

        case PrimitiveType::PRIMITIVE_TORUS:
            // TODO: torus?
            break;

        case PrimitiveType::PRIMITIVE_SPHERE:
            hit = CS123Sphere::castRay(p, ray, min_t, normal, uv);
            break;

        case PrimitiveType::PRIMITIVE_MESH:
            // TODO: mesh?
            break;
    }

    if (hit) {
        auto m = glm::transpose(glm::mat3(glm::inverse(m_modelTransform)));
        normal = glm::normalize(glm::vec4(m * glm::vec3(normal), 0));
    }
    return hit;
}
