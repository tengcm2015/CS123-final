#ifndef CS123CYLINDER_H
#define CS123CYLINDER_H

#include "CS123Mesh.h"

class CS123Cylinder : public CS123Mesh {
public:
    explicit CS123Cylinder(int stack = 15, int slice = 15,
                           const glm::mat4 &transform = {});
    ~CS123Cylinder() override;

    static bool castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv);
};

#endif // CS123CYLINDER_H
