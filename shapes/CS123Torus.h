#ifndef CS123TORUS_H
#define CS123TORUS_H

#include "CS123Mesh.h"

class CS123Torus : public CS123Mesh {
public:
    explicit CS123Torus(int slice = 15, int ringSlice = 15, float ringRadius = 15.0,
                        const glm::mat4 &transform = glm::mat4());
    ~CS123Torus() override;
};

#endif // CS123TORUS_H
