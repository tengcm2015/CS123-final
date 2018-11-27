#ifndef CS123CONE_H
#define CS123CONE_H

#include "CS123Mesh.h"

class CS123Cone : public CS123Mesh {
public:
    explicit CS123Cone(int stack = 15, int slice = 15,
                       const glm::mat4 &transform = glm::mat4());
    ~CS123Cone() override;

    static bool castRay(const glm::vec4 &p, const glm::vec4 &ray, float &min_t, glm::vec4 &normal, glm::vec2 &uv);
};

#endif // CS123CONE_H
