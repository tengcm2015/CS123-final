#ifndef CS123SPHERE_H
#define CS123SPHERE_H

#include "CS123Mesh.h"

class CS123Sphere : public CS123Mesh {
public:
    explicit CS123Sphere(int stack = 15, int slice = 15,
                         const glm::mat4 &transform = glm::mat4());
    ~CS123Sphere() override;

    static bool castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv);
};

#endif // CS123SPHERE_H
