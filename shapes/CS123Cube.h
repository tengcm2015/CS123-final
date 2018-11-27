#ifndef CS123CUBE_H
#define CS123CUBE_H

#include "CS123Mesh.h"

class CS123Cube : public CS123Mesh {
public:
    explicit CS123Cube(int stack = 15,
                       const glm::mat4 &transform = glm::mat4());
    ~CS123Cube() override;

    static bool castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv);
};

#endif // CS123CUBE_H
