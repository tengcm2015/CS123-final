#ifndef CS123PLANE_H
#define CS123PLANE_H

#include "CS123Mesh.h"

class CS123Quad : public CS123Mesh {
public:
    explicit CS123Quad(int stack = 15,
                       const glm::mat4 &transform = glm::mat4());
    ~CS123Quad() override;

    static bool castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv);
};


#endif //CS123PLANE_H
